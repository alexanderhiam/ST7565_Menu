/*
 ST7565_Menu.cpp  -  Alex Hiam  -  rev 0.2
 Created: 11/2011
 
 A menu based user interface framework designed for the Adafruit     
 ST7565 Arduino library.


 Copyright (c) 2011,2012 - Alex Hiam - <ahiam@marlboro.edu>
 http://www.alexanderhiam.com

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

#include "ST7565_Menu.h"
#include <string.h>
#include <stdlib.h>

Menu::Menu(uint8_t up_pin, uint8_t down_pin, uint8_t select_pin, ST7565 *lcd) {
  _up = up_pin;
  _down = down_pin;
  _select = select_pin;
  _up_pressed = _down_pressed = _select_pressed = false;
  _lcd = lcd;
  pinMode(_up, INPUT);
  pinMode(_down, INPUT);
  pinMode(_select, INPUT); 
  _title[0]='\0';  // So it will be blank if not set
  // Set all flags/counters to 0:
  _n_items = _current_line = _draw_index = _item_index = 0;
  _has_draw_funct = _has_timeout_funct = false;
  inactive_count = _active_timeout = 0;
}

void Menu::update() {
  if (!digitalRead(_up)) {
    if (!_up_pressed) {  // Only move once until button release or MAX_HOLD_COUNT reached
      if (_current_line > 0) _current_line--;
      else scroll(-1); // We're at the top line, scroll up if possible
      if (_item_index > 0) {
        _item_index--;
      }
      _up_pressed = true; 
    }
    else _up_hold_counter++;
    if (_up_hold_counter >= MAX_HOLD_COUNT) {
      _up_pressed = false;
      _up_hold_counter = HOLD_SCROLL_SPEED;
    }
  }
  else {
    _up_pressed = false;
    _up_hold_counter = 0;
  }
  if (!digitalRead(_down)) {
    if (!_down_pressed) {  // Wait until button release or max hold time reached
      if ((_current_line < N_LINES-2) & (_current_line < _n_items-1)) _current_line++;
      else scroll(1); // We're at the bottom line, scroll down if possible
      if (_item_index < _n_items-1) {
        _item_index++;
      }
      _down_pressed = true; 
    }
    else _down_hold_counter++;
    if (_down_hold_counter >= MAX_HOLD_COUNT) {
      _down_pressed = false;
      _down_hold_counter = HOLD_SCROLL_SPEED; 
    }
  }
  else {
    _down_pressed = false;
    _down_hold_counter = 0;
  }

  if (!digitalRead(_select)) {
    if (!_select_pressed) {  // Only selet once until button release
      if (_menu_items[_item_index].pass_value) {
        _menu_items[_item_index].value_function(_menu_items[_item_index].value);
      }
      else if (_menu_items[_item_index].funct) _menu_items[_item_index].function();
      _select_pressed = true; 
    }
  }
  else _select_pressed = false;
  draw();

  if (_has_timeout_funct) {
    // Inactive this time through loop:
    if (!(_up_pressed|_down_pressed|_select_pressed)) inactive_count++; 
    else inactive_count = 0;  // A button was pressed, reset counter
    if (inactive_count >= _active_timeout) { 
      // Timeout reached, call timeout funtion:
      _timeout_function();
      inactive_count = 0;
    }
  }
}

void Menu::set_title(char *label) {
  strcpy(_title, label); // First line of display, blank if not set
}

void Menu::add_item(char *label) {
  if (_n_items >= MAX_ITEMS) return;
  if (strlen(label) > LABEL_LEN) return;
  MenuItem i;
  strcpy(i.label, label);
  i.pass_value = i.funct = false; // Calls no function
  _menu_items[_n_items++] = i;
}

void Menu::add_item(char *label, void (*function)(void)) {
  if (_n_items >= MAX_ITEMS) return;
  if (strlen(label) > LABEL_LEN) return;
  MenuItem i;
  strcpy(i.label, label);
  i.pass_value = false;   // No value to pass to function
  i.funct = true;         // Has a function to call on select
  i.function = function;  // Function to call
  _menu_items[_n_items++] = i;
}

void Menu::add_item(char *label, int value, void (*function)(int)) {
  if (_n_items >= MAX_ITEMS) return;
  if (strlen(label) > LABEL_LEN) return;
  MenuItem i;
  strcpy(i.label, label);
  i.pass_value = i.funct = true; // Has function and should pass it a value
  i.value = value;               // Value to pass to function on select
  i.value_function = function;   // Function to call
  _menu_items[_n_items++] = i;
}

void Menu::add_draw_function(void (*function)(void)) {
  _has_draw_funct = 1;       // Should call function before (*lcd).display()
  _draw_function = function; // Function to call
}

void Menu::add_timeout_function(int timeout, void (*function)(void)) {
  _has_timeout_funct = 1;
  _active_timeout = timeout;    // Number of loops without button press
                                //  before calling timeout function
  _timeout_function = function; // Function to call
}

void Menu::scroll(int8_t dir) {
  // Only scrolls screen if possible, doesn't change _item_index
  _draw_index = _draw_index + dir;
  if (_draw_index < 0) _draw_index = 0;
  if (_n_items >= N_LINES) {
    if (_draw_index > _n_items-N_LINES+1) _draw_index = _n_items-N_LINES+1;
  }
  else _draw_index = 0;
}

void Menu::draw() {
  uint8_t i, label_len;
  (*_lcd).clear();
  (*_lcd).drawstring(0, 0, _title);(*_lcd).drawstring(LEFT_MARGIN, i+1,
                                         _menu_items[_draw_index+i].label);    
  i=0;
  while ((i<N_LINES-1) & (i<_n_items)) {
    (*_lcd).drawstring(LEFT_MARGIN, i+1, _menu_items[_draw_index+i].label); 
    i++;
  }

  // Draw arrow to indicate current item:
  (*_lcd).drawline(0, 11+(8*_current_line), LEFT_MARGIN-5, 
                                                11+(8*_current_line), BLACK);
  (*_lcd).drawline(LEFT_MARGIN-5, 8+(8*_current_line), LEFT_MARGIN-2, 
                                                11+(8*_current_line), BLACK);
  (*_lcd).drawline(LEFT_MARGIN-5, 14+(8*_current_line), LEFT_MARGIN-2,
                                                11+(8*_current_line), BLACK);
  (*_lcd).drawline(LEFT_MARGIN-5, 14+(8*_current_line), LEFT_MARGIN-5,
                                                8+(8*_current_line), BLACK);

  
  // Draw up arrow if there are items above view
  if (_draw_index > 0) {
    (*_lcd).drawline(123, 11, 125, 8, BLACK);
    (*_lcd).drawline(127, 11, 125, 8, BLACK);
  }  
  // Draw down arrow if there are items below view
  if ((_n_items - _draw_index) >= N_LINES) {
    (*_lcd).drawline(123, 60, 125, 63, BLACK);
    (*_lcd).drawline(127, 60, 125, 63, BLACK);
  }

  if (_has_draw_funct) _draw_function();
  (*_lcd).display();
}

void Menu::clear() {
  _n_items = _current_line = _draw_index = _item_index = 0;
  _has_draw_funct = _has_timeout_funct = _active_timeout = inactive_count = 0;
  _title[0]='\0';  // So it will be blank if set_title() not called
  (*_lcd).clear();
  //(*_lcd).display();
}
