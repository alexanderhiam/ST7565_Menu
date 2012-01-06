/*
 simple_menu.pde - 11/2011
 Basic example of ST7565_Menu Arduino library.
 
 Remember the ST7565 uses 3.3v, so a level shifter
 is needed if using a standard Arduino board.
 See Adafruit tutorial for more details:
  http://www.ladyada.net/learn/lcd/st7565.html
*/

#include <ST7565.h> // Adafruit LCD library
#include <ST7565_Menu.h> 

// Menu controls:
#define UP_PIN     3  
#define DOWN_PIN   4  
#define SELECT_PIN 2  

// LCD pins:
#define BACKLIGHT_LED 10
#define LCD_SID  9
#define LCD_SCLK 8
#define LCD_A0   7 
#define LCD_RST  6
#define LCD_CS   5

// Must create an ST7565 instance before Menu:
ST7565 glcd(LCD_SID, LCD_SCLK, LCD_A0, LCD_RST, LCD_CS);

// Create Menu with control pins and address of glcd:
Menu menu(UP_PIN, DOWN_PIN, SELECT_PIN, &glcd);

int backlight_state = 0; // 0-Off, 1-On

void setup() {
  pinMode(BACKLIGHT_LED, OUTPUT);
  glcd.begin(0x18); // Initialise LCD 
  delay(10); // Just to be safe
  toggle_backlight();
  show_menu();  // Create menu
}

void show_menu() {
  // Title always shown on first line:
  menu.set_title("==Simple Menu==");
  
  // Create an item which will call toggle_backlight()
  // when selected:
  menu.add_item("Toggle Backlight", toggle_backlight);
  
  // Add a couple items that do nothing when selected:
  menu.add_item("Menu Item 2");
  menu.add_item("Menu Item 3");
}

void toggle_backlight() {
  backlight_state = backlight_state^1;
  digitalWrite(BACKLIGHT_LED, backlight_state);
}

void loop() {
  // Now we just need to update the menu each time 
  // through the main loop, it takes care of the 
  // rest for us!
  menu.update();
}


