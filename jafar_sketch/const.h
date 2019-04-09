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

#ifndef const_h
#define const_h

#define FORCE_FIRST_MENU_ITEM //force always the first menu item (last freq used)
#define STANDALONE

#define USE_48CH
#define USE_SCANNER


#define CHANNEL_MIN 0
#define NUM_BANDS 6
#define CHANNEL_MAX 48

#define FLIP_SCREEN

#define USE_OSD

#define LOOPTIME 100

#define JAFARE_DEBOUCE_TIME 180

#define TIMER_INIT_VALUE 5.0
#define CH1 2
#define CH2 3
#define CH3 4

//calibration stored values
#define EEPROM_ADR_RSSI_MIN_L 2
#define EEPROM_ADR_RSSI_MIN_H 3
#define EEPROM_ADR_RSSI_MAX_L 4
#define EEPROM_ADR_RSSI_MAX_H 5

#define EEPROM_ADDR_LAST_BAND_ID 15
#define EEPROM_ADDR_LAST_FREQ_ID 16

#define EEPROM_ADDR_START_LOG 20

#define RX_A 1

//diplay dependant
#define D_COL 120
#define D_ROW 96
#define MENU_Y_SIZE 10

//SPI related
#define spiDataPin 11
#define spiClockPin 13

#define SPI_CSA 10//receiver SPI Select pin
#define rssiA A2  //RSSI input

#define SW_CTRL1 5
#define SW_CTRL2 6

//#define BUZPIN 0 //digitalWrite(BUZPIN, HIGH);TV.delay(50);digitalWrite(BUZPIN, LOW);pinMode(BUZPIN, OUTPUT); //UP digitalWrite(BUZPIN,LOW);
#define SELECT_OSD {digitalWrite(SW_CTRL1, HIGH);digitalWrite(SW_CTRL2, HIGH);}
#define SELECT_A {digitalWrite(SW_CTRL1, LOW);  digitalWrite(SW_CTRL2, HIGH);}

#define RX_HYST 0 //~10%

// Channels with their Mhz Values
const uint16_t channelFreqTable[] PROGMEM = {
  // Channel 1 - 8
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // Band A
  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // Band B
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // Band E
  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // Band F / Airwave
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // Raceband 1
  5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621  // Raceband 2
};

// All Channels of the above List ordered by Mhz
const uint8_t channelList[] PROGMEM = {
  19, 32, 18, 17, 33, 16, 7, 34, 8, 24, 6, 9, 25, 5, 35, 10, 26, 4, 11, 27, 3, 36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0, 15, 31, 38, 20, 21, 39, 22, 23
};

const uint8_t channelNames[] PROGMEM = {
  0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
  0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8,
  0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8,
  0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
  0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8,
  0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8
};

//menu entry positions
#define NUM_MENU_ITEMS 9

#define  LAST_USED_POS 0
#define  BAND_A_POS 1
#define  BAND_B_POS 2
#define  BAND_E_POS 3
#define  BAND_F_POS 4
#define  BAND_R1_POS 5
#define  BAND_R2_POS 6
#define  SCANNER_POS 7
#define  AUTOSCAN_POS 8

uint8_t _init_selection;

uint8_t inline compute_position(const uint8_t _pos) {
  return ((_init_selection + _pos) % NUM_MENU_ITEMS);
}
#endif
