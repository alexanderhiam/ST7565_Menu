/*
 menu_test.pde - 11/2011
 A more complicated example of ST7565_Menu Arduino library.
 
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
#define BACKLIGHT_LED 10  // Must be a PWM pin
#define LCD_SID  9
#define LCD_SCLK 8
#define LCD_A0   7 
#define LCD_RST  6
#define LCD_CS   5

// Must create an ST7565 instance before Menu:
ST7565 glcd(LCD_SID, LCD_SCLK, LCD_A0, LCD_RST, LCD_CS);

// Create Menu with control pins and address of glcd:
Menu menu(UP_PIN, DOWN_PIN, SELECT_PIN, &glcd);

//Backlight PWM values:
uint8_t brightness_levels[7] = { 0, 20, 50, 90, 130, 190, 250 };
uint8_t brightness_index; // Index of brightness_levels

void setup() {
  pinMode(BACKLIGHT_LED, OUTPUT);
  glcd.begin(0x18); // Initialise LCD
  delay(10); // Just to be safe
  brightness_index = 1; // Initial brightness
  set_brightness(0); 
  show_main(); // Draw main menu
}

// Draw main menu screen:
void show_main() {
  menu.clear(); // Clear menu and display
  
  // If menu.update() called 70 times, light_of() will be called:
  menu.add_timeout_function(70, light_off); 

  // Title always shown on first line:
  menu.set_title("Example Menu:");
  
  // Add items with function to be called when selected:
  menu.add_item("Read Analog", show_analog);
  menu.add_item("Set Brightness", show_brightness);
  menu.add_item("Scroll Test", scroll_test);
}

// Draw analog display menu:
void show_analog() {
  menu.clear(); 
  
  // draw_analog() will be called after menu items are
  // drawn and before glcd.display() is called:
  menu.add_draw_function(draw_analog);
  
  menu.set_title("Analog Value:");
  menu.add_item("Back", show_main);
}

// Draw brightness menu:
void show_brightness() {
  menu.clear();
  
  // Back to main menu after 40 a inactive loops:
  menu.add_timeout_function(40, show_main);
  
  menu.add_draw_function(draw_brightness);
  menu.set_title("Brightness:");
  
  // These items will pass their integer values
  // (1 and -1) to set_brightness:
  menu.add_item("Up", 1, set_brightness);
  menu.add_item("Down", -1, set_brightness);
  
  menu.add_item("Back", show_main);
}

// Fill a menu to demonstrate scrolling:
void scroll_test() {
  uint8_t i;
  char value[12]; // To hold counter value
  char label[22]; // To hold item label
  menu.clear();
  menu.add_timeout_function(40, show_main);
  menu.set_title("Scroll Test:");
  menu.add_item("Back", show_main);
  for (i=1; i<=MAX_ITEMS-2; i++) {
    strcpy(label, "Menu Item "); // First part of label
    itoa(i, value, 10);   // Get counter value
    strcat(label, value); // Append counter string
    menu.add_item(label); // Add it
  }
  menu.add_item("Back", show_main);
}

// Convert and display ADC values from A0 and A1:
void draw_analog() {
  char buffer[12];
  int value;
  value = analogRead(A0);
  itoa(value, buffer, 10);
  glcd.drawstring(20, 3, "A0:");
  glcd.drawstring(44, 3, buffer);
  
  value = analogRead(A1);
  itoa(value, buffer, 10);
  glcd.drawstring(20, 4, "A1:");
  glcd.drawstring(44, 4, buffer);
}

// Lower brightness if dir<0, raise if dir>0
void set_brightness(int dir) {
  if (dir < 0) {
    if (brightness_index > 0) brightness_index--;
  }
  if (dir > 0) {
    if (brightness_index < 6) brightness_index++;
  }
  analogWrite(BACKLIGHT_LED, brightness_levels[brightness_index]);
}

// Draw lower section of brightness screen:
void draw_brightness() {
  char buf[12];
  // Just display brightness_index as brightness level:
  itoa(brightness_index, buf, 10); 
  glcd.drawstring(LEFT_MARGIN+10, 5, buf);
  // Visual level display:
  glcd.drawrect(LEFT_MARGIN+17, 40, 60, 8, BLACK);
  glcd.fillrect(LEFT_MARGIN+17, 40, 10*brightness_index, 8, BLACK);
}

void light_off() {
  uint8_t brightness = brightness_index; // Save current level
  brightness_index = 0;
  set_brightness(0);
  // Hold until button press then reset brightnes:
  while (digitalRead(UP_PIN)&digitalRead(DOWN_PIN)&digitalRead(SELECT_PIN));
  brightness_index = brightness; // 
  set_brightness(0);
}

void loop() {
  // Now we just need to update the menu each time 
  // through the main loop, it takes care of the 
  // rest for us!
  menu.update();
}

