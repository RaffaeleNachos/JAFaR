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

inline uint8_t readSwitch() {
  int but_up = digitalRead(CH1);
  int but_ent = digitalRead(CH2);
  int but_down = digitalRead(CH3);

  if (but_up == LOW && menu_pos < 8)
    menu_pos++;

  if (but_down == LOW && menu_pos > 0)
    menu_pos--;

  if (but_ent == LOW)
    timer = 0;

  return menu_pos;
}

void set_and_wait(uint8_t band, uint8_t menu_pos) {
  u8 current_rx;
  uint8_t last_post_switch = readSwitch();


  //no more RAM at this point :( lets consume less...
  TV.end();
  TV.begin(PAL, D_COL / 2, D_ROW / 2);
  TV.select_font(font4x6);
  TV.printPGM(0, 10, PSTR("PLEASE\nWAIT..."));
  
  SELECT_A;
  current_rx = RX_A;

  rx5808.setFreq(pgm_read_word_near(channelFreqTable + (8 * band) + menu_pos)); //set the selected freq

  //clear memory for log
#ifdef ENABLE_RSSILOG
  uint8_t sample = 0;
  long g_log_offset = 0;
  for (g_log_offset = 0 ; g_log_offset < EEPROM.length() / 2 ; g_log_offset++) {
    EEPROM.write(EEPROM_ADDR_START_LOG + g_log_offset, 0);
  }
  g_log_offset = 0;
#endif

  //save band and freq as "last used"
  EEPROM.write(EEPROM_ADDR_LAST_FREQ_ID, menu_pos); //freq id
  EEPROM.write(EEPROM_ADDR_LAST_BAND_ID, band); //channel name

  //MAIN LOOP - change channel and log
  while (1) {
    menu_pos = readSwitch();

    if (last_post_switch != menu_pos) { //something changed by user
      
      int i = 0;
      TV.clear_screen();
      for (i = 0; i < 8; i++) {
        TV.print(0, i * 6, pgm_read_byte_near(channelNames + (8 * band) + i), HEX); //channel freq
        TV.print(10, i * 6, pgm_read_word_near(channelFreqTable + (8 * band) + i), DEC); //channel name
      }
      TV.draw_rect(30, menu_pos * 6 , 5, 5,  WHITE, INVERT); //current selection
      SELECT_OSD;
      TV.delay(1000);
      SELECT_A;
      current_rx = RX_A;
      rx5808.setFreq(pgm_read_word_near(channelFreqTable + (8 * band) + menu_pos)); //set the selected freq

      EEPROM.write(EEPROM_ADDR_LAST_FREQ_ID, menu_pos);
      
      jafar_delay(JAFARE_DEBOUCE_TIME); //debounce

    }
    last_post_switch = menu_pos;
  } //end of loop

}
