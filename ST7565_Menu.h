/*
 ST7565_Menu.h  -  Alex Hiam  -  rev 0.1
 Created: 11/2011
 
 A menu based user interface framework designed for the Adafruit     
 ST7565 Arduino library.         

 
 Copyright (c) 2011,2012 - Alex Hiam - <ahiam@marlboro.edu>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef _ST7565_MENU_H
#define _ST7565_MENU_H

#include "WProgram.h"
#include "../ST7565/ST7565.h"  // Change accordingly

//--- Configuration: ---
#define LEFT_MARGIN       10 // Should be >4
#define MAX_ITEMS         12
#define MAX_HOLD_COUNT    3  // Loops of holding up or down button
                             //  before continuing to scroll 
#define HOLD_SCROLL_SPEED 3
//----------------------

//--- Constants: ---
#define N_LINES 8
#define LABEL_LEN 19
#define ST7565_MENU_VERSION "Menu UI v0.1"
//------------------


// Struct used for all types of menu items
typedef struct {
  char label[LABEL_LEN+1];
  int value, update_counter;
  bool pass_value, funct;
  void (*function)(void);
  char* (*data_function)(void);
  void (*value_function)(int);
} MenuItem;


class Menu {
  public:
    Menu(uint8_t up_pin, uint8_t down_pin, uint8_t select_pin, ST7565 *lcd);
    void update();
    void set_title(char *title);
    void add_item(char *label);
    void add_item(char *label, void (*function)(void));
    void add_item(char *label, int value, void (*function)(int));
    void add_draw_function(void (*function)(void));
    void add_timeout_function(int timeout, void (*function)(void));
    void draw();
    void clear();
    int inactive_count;
  private:
    MenuItem _menu_items[MAX_ITEMS];
    void scroll(int8_t dir);
    ST7565 *_lcd;
    void (*_draw_function)(void);
    void (*_timeout_function)(void);
    bool _up_pressed, _down_pressed, _select_pressed, _has_draw_funct, 
         _has_timeout_funct;
    uint8_t _up, _down, _select, _n_items, _item_index, _current_line,
            _up_hold_counter, _down_hold_counter;
    int8_t _draw_index;
    int _active_timeout;
    char _title[LABEL_LEN];
};

#endif
