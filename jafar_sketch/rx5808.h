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

#ifndef rx5808_h
#define rx5808_h

#include "Arduino.h"
#include "const.h"

class RX5808
{
  public:
    RX5808(uint8_t RSSIpin, uint8_t CSpin);
    uint16_t getVal(uint16_t band, uint16_t channel, uint16_t norm);
    uint16_t getVal(uint16_t pos, uint16_t norm);
    uint16_t getMaxPosBand(uint8_t band);
    uint16_t getMaxValBand(uint8_t band, uint16_t norm);
    uint16_t getMinPosBand(uint8_t band);
    uint16_t getMaxPos();
    uint16_t getMinPos();
    void scan();
    void init();
    void calibration();
    void setFreq(uint32_t freq);
    void abortScan();
    uint16_t getfrom_top8(uint8_t index);
    void compute_top8(void);
    uint16_t getRssi(uint16_t channel);
    uint16_t getRssiMin();
    uint16_t getRssiMax();
    void setRssiMax(uint16_t new_max);
    void setRssiMin(uint16_t new_min);
    uint16_t getCurrentRSSI();
    void setRSSIMinMax();

  private:
    uint16_t _readRSSI();
    void _wait_rssi();
    uint8_t _rssiPin;
    uint8_t _csPin;
    uint8_t _stop_scan;
    uint16_t scanVec[CHANNEL_MAX];
    uint16_t scanVecTop8[8];

    void serialEnable(const uint8_t);
    void serialSendBit(const uint8_t);

    //default values used for calibration
    uint16_t rssi_min;
    uint16_t rssi_max;

};

#endif
