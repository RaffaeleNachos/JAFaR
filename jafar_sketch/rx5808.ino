/*
This file is part of Fatshark© goggle rx module project (JAFaR).

    JAFaR is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    JAFaR is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.

    Copyright © 2016 Michele Martinelli
  */

#include <Wire.h>
#include <EEPROM.h>
#include "rx5808.h"
#define RSSI_THRESH (scanVec[getMaxPos()]-5) //strong channels are near to the global max

RX5808::RX5808(uint16_t RSSIpin, uint16_t CSpin) {
  _rssiPin = RSSIpin;
  _csPin = CSpin;
  _stop_scan = 0;
}

uint16_t RX5808::getRssi(uint16_t channel) {
  return scanVec[channel];
}

//stop scan
void RX5808::abortScan(void) {
  _stop_scan = 1;
}
//get next strong rssi channel
uint16_t RX5808::getNext(uint16_t channel) {
  channel = (channel + 1) % CHANNEL_MAX;
  for (uint16_t  complete_iter = CHANNEL_MAX - CHANNEL_MIN; complete_iter != 0; complete_iter-- ) {
    if (scanVec[channel] > RSSI_THRESH) //new over threashold
      return channel;
    channel = (channel + 1) % CHANNEL_MAX;
  }

  return channel;
}

//get the rssi value of a certain channel of a band and map it to 1...norm
uint16_t RX5808::getVal(uint16_t band, uint16_t channel, uint8_t norm) {
  return map(scanVec[8 * band + channel], 1, BIN_H, 1, norm);
}

//get the maximum rssi value for a certain band and map it to 1...norm
uint16_t RX5808::getMaxValBand(uint8_t band, uint8_t norm) {
  uint16_t _chan;
  uint16_t maxVal = 0, maxPos = 8 * band;
  for (_chan = 8 * band; _chan < 8 * band + 8; _chan++) {
    if (maxVal < scanVec[_chan]) { //new max
      maxPos = _chan;
      maxVal = scanVec[_chan];
    }
  }
  return map(maxVal, 1, BIN_H, 1, norm);
}

//get the channel with max rssi value for a certain band
uint16_t RX5808::getMaxPosBand(uint8_t band) {
  uint16_t _chan;
  uint16_t maxVal = 0, maxPos = 8 * band;
  for (_chan = 8 * band; _chan < 8 * band + 8; _chan++) {
    if (maxVal < scanVec[_chan]) { //new max
      maxPos = _chan;
      maxVal = scanVec[_chan];

    }
  }
  return maxPos;
}

//get the minimum rssi value for a certain band
uint16_t RX5808::getMinPosBand(uint8_t band) {
  uint16_t _chan;
  uint16_t minVal = 1000, minPos = 8 * band;
  for (_chan = 8 * band; _chan < 8 * band + 8; _chan++) {
    if (minVal > scanVec[_chan]) { //new max
      minPos = _chan;
      minVal = scanVec[_chan];

    }
  }
  return minPos;
}

//get global max
uint16_t RX5808::getMaxPos() {
  uint8_t _chan;
  uint16_t maxVal = 0, maxPos = 0;
  for (_chan = CHANNEL_MIN; _chan < CHANNEL_MAX; _chan++) {
    if (maxVal < scanVec[_chan]) { //new max
      maxPos = _chan;
      maxVal = scanVec[_chan];

    }
  }
  return maxPos;
}

//get global min
uint16_t RX5808::getMinPos() {
  uint8_t _chan;
  uint16_t minVal = 1000, minPos = 0;
  for (_chan = CHANNEL_MIN; _chan < CHANNEL_MAX; _chan++) {
    if (minVal > scanVec[_chan]) { //new max
      minPos = _chan;
      minVal = scanVec[_chan];

    }
  }
  return minPos;
}

uint16_t RX5808::getCurrentRSSI() {
  return _readRSSI();
}

void RX5808::init() {
  pinMode (_csPin, OUTPUT);
  pinMode (_rssiPin, INPUT);

  rssi_min = ((EEPROM.read(EEPROM_ADR_RSSI_MIN_H) << 8) | (EEPROM.read(EEPROM_ADR_RSSI_MIN_L)));
  rssi_max = ((EEPROM.read(EEPROM_ADR_RSSI_MAX_H) << 8) | (EEPROM.read(EEPROM_ADR_RSSI_MAX_L)));

  /*
    digitalWrite(_csPin, LOW);
    SPI.transfer(0x10);
    SPI.transfer(0x01);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    digitalWrite(_csPin, HIGH);
  */
  scan(1, BIN_H);
}



//do a complete scan and normalize all the values
void RX5808::scan(uint16_t norm_min, uint16_t norm_max) {

  for (uint16_t _chan = CHANNEL_MIN; _chan < CHANNEL_MAX; _chan++) {
    if (_stop_scan) {
      _stop_scan = 0;
      return;
    }

    uint32_t freq = pgm_read_word_near(channelFreqTable + _chan);
    setFreq(freq);
    _wait_rssi();

    uint16_t rssi =  _readRSSI();
#ifdef DEBUG
    Serial.print(_chan, DEC);
    Serial.print("\t");
    Serial.print(freq, DEC);
    Serial.print("\t");
    Serial.println(rssi, DEC);
    delay(1000);
#endif

    rssi = constrain(rssi, rssi_min, rssi_max);
    rssi = map(rssi, rssi_min, rssi_max, norm_min, norm_max);   // scale from 1..100%
    scanVec[_chan] = rssi;


  }
}

//same as scan, but raw values, used for calibration
void RX5808::_calibrationScan() {
  for (uint16_t _chan = CHANNEL_MIN; _chan < CHANNEL_MAX; _chan++) {

    uint32_t freq = pgm_read_word_near(channelFreqTable + _chan);
    setFreq(freq);
    _wait_rssi();
    scanVec[_chan] = _readRSSI();
  }
}

void RX5808::_wait_rssi() {
  // 30ms will to do a 32 channels scan in 1 second
#define MIN_TUNE_TIME 30
  delay(MIN_TUNE_TIME);
}

uint16_t RX5808::_readRSSI() {
  uint32_t  sum = 0;
  for (uint8_t i = 0; i < 10; i++)
  {
    sum += analogRead(_rssiPin);
    delay(2);
  }
  return sum / 10; // average
}

//compute the min and max RSSI value and store the values in EEPROM
void RX5808::calibration() {
  int i = 0, j = 0;
  uint16_t  rssi_setup_min = 1024, minValue = 1024;
  uint16_t  rssi_setup_max = 0, maxValue = 0;

  for (j = 0; j < 5; j++) {
    _calibrationScan();

    for (i = CHANNEL_MIN; i < CHANNEL_MAX; i++) {
      uint16_t rssi = scanVec[i];

      minValue = min(minValue, rssi);
      maxValue = max(maxValue, rssi);
    }

    rssi_setup_min = min(rssi_setup_min, minValue); //?minValue:rssi_setup_min;
    rssi_setup_max = max(rssi_setup_max, maxValue); //?maxValue:rssi_setup_max;
  }

  // save 16 bit
  EEPROM.write(EEPROM_ADR_RSSI_MIN_L, (rssi_setup_min & 0xff));
  EEPROM.write(EEPROM_ADR_RSSI_MIN_H, (rssi_setup_min >> 8));
  // save 16 bit
  EEPROM.write(EEPROM_ADR_RSSI_MAX_L, (rssi_setup_max & 0xff));
  EEPROM.write(EEPROM_ADR_RSSI_MAX_H, (rssi_setup_max >> 8));

  rssi_min = ((EEPROM.read(EEPROM_ADR_RSSI_MIN_H) << 8) | (EEPROM.read(EEPROM_ADR_RSSI_MIN_L)));
  rssi_max = ((EEPROM.read(EEPROM_ADR_RSSI_MAX_H) << 8) | (EEPROM.read(EEPROM_ADR_RSSI_MAX_L)));


  delay(3000);

  return;
}

#ifdef USE_NATIVE_SPI
//set a certain frequency for the RX module
void RX5808::setFreq(uint32_t freq) {
  byte data0 = 0;
  byte data1 = 0;
  byte data2 = 0;
  byte data3 = 0;

  uint32_t Delitel = (freq - 479) / 2;

  byte DelitelH = Delitel >> 5;
  byte DelitelL = Delitel & 0x1F;

  data0 = DelitelL * 32 + 17;
  data1 = DelitelH * 16 + DelitelL / 8;
  data2 = DelitelH / 16;
  data3 = 0;

  digitalWrite(_csPin, LOW);
  SPI.transfer(data0);
  SPI.transfer(data1);
  SPI.transfer(data2);
  SPI.transfer(data3);
  digitalWrite(_csPin, HIGH);
}

#else //DONT USE NATIVE SPI
void RX5808::setFreq(uint32_t freq) {
  uint8_t i;
  uint16_t channelData;

  uint32_t _if = (freq - 479);

  uint32_t N = floor(_if / 64);
  uint32_t A = floor((_if / 2) % 32);
  channelData = (N << 7) | (A & 0x7F);

#ifdef DEBUG
  Serial.print("N: ");
  Serial.println(N, DEC);

  Serial.print("A: ");
  Serial.println(A, DEC);

  Serial.print("setting ");
  Serial.println(channelData, HEX);
#endif

  //REGISTER 1 - selection
  // bit bash out 25 bits of data
  // Order: A0-3, !R/W, D0-D19
  // A0=0, A1=0, A2=0, A3=1, RW=0, D0-19=0
  SERIAL_ENABLE_HIGH();
  delayMicroseconds(1);
  SERIAL_ENABLE_LOW();

  SERIAL_SENDBIT0();
  SERIAL_SENDBIT0();
  SERIAL_SENDBIT0();
  SERIAL_SENDBIT1();

  SERIAL_SENDBIT0();

  // remaining zeros
  for (i = 20; i > 0; i--)
    SERIAL_SENDBIT0();

  // Clock the data in
  SERIAL_ENABLE_HIGH();
  //delay(2);
  delayMicroseconds(1);
  SERIAL_ENABLE_LOW();

  // Second is the channel data from the lookup table
  // 20 bytes of register data are sent, but the MSB 4 bits are zeros
  // register address = 0x1, write, data0-15=channelData data15-19=0x0
  SERIAL_ENABLE_HIGH();
  SERIAL_ENABLE_LOW();

  // Register 0x1
  SERIAL_SENDBIT1();
  SERIAL_SENDBIT0();
  SERIAL_SENDBIT0();
  SERIAL_SENDBIT0();

  // Write to register
  SERIAL_SENDBIT1();

  // D0-D15
  //   note: loop runs backwards as more efficent on AVR
  for (i = 16; i > 0; i--)
  {
    // Is bit high or low?
    if (channelData & 0x1)
    {
      SERIAL_SENDBIT1();
    }
    else
    {
      SERIAL_SENDBIT0();
    }

    // Shift bits along to check the next one
    channelData >>= 1;
  }

  // Remaining D16-D19
  for (i = 4; i > 0; i--)
    SERIAL_SENDBIT0();

  // Finished clocking data in
  SERIAL_ENABLE_HIGH();
  delayMicroseconds(1);
  //delay(2);

  //  digitalWrite(_csPin, LOW);
  digitalWrite(spiClockPin, LOW);
  digitalWrite(spiDataPin, LOW);
}


void RX5808::SERIAL_SENDBIT1()
{
  digitalWrite(spiClockPin, LOW);
  delayMicroseconds(1);

  digitalWrite(spiDataPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(spiClockPin, HIGH);
  delayMicroseconds(1);

  digitalWrite(spiClockPin, LOW);
  delayMicroseconds(1);
}

void RX5808::SERIAL_SENDBIT0()
{
  digitalWrite(spiClockPin, LOW);
  delayMicroseconds(1);

  digitalWrite(spiDataPin, LOW);
  delayMicroseconds(1);
  digitalWrite(spiClockPin, HIGH);
  delayMicroseconds(1);

  digitalWrite(spiClockPin, LOW);
  delayMicroseconds(1);
}

void RX5808::SERIAL_ENABLE_LOW()
{
  delayMicroseconds(1);
  digitalWrite(_csPin, LOW);
  delayMicroseconds(1);
}

void RX5808::SERIAL_ENABLE_HIGH()
{
  delayMicroseconds(1);
  digitalWrite(_csPin, HIGH);
  delayMicroseconds(1);
}
#endif
