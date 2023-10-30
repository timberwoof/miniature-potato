#define ioaddress 0x20

// Bits for the i2c interface
#define bit1 B10000000
#define bit2 B01000000
#define bit3 B00100000
#define bit4 B00010000
#define bit5 B00001000
#define bit6 B00000100

// Relay patterns
#define btoa bit1 + bit5
#define atob bit2 + bit4
#define ctoa bit1 + bit6
#define atoc bit3 + bit4
#define ctob bit2 + bit6
#define btoc bit3 + bit5

// LCD panel buttons (from ButtonTest sketch)
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// definitions for operating parameters
// Delay after setting relays and reading AD value.
// Need to give the DA time ti settle.  
#define read_delay 10 // 5 fails, 6 works.
// Delay after reading and setting next relay combination
#define relay_delay 90 // Fast sounds scary. 

// Threshold for value from AD converter.
// Low is ~ 100 and high is ~250 so ~150 works. 
#define threshold 150 

// Address for PCF interface. 
#define PCF_ADDRESS 0x20

// Setup libraries
#include <Wire.h>
#include "PCF8575.h"
#include <LiquidCrystal.h>
PCF8575 PCF(PCF_ADDRESS);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  // Set up LCD panel with initial title.
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("TransistorChecker");
  lcd.setCursor(0, 1);
  lcd.print("Press a Button");

  // Start the two-wire interface
  Wire.begin();
  // Set the PCF for output. 
  PCF.write16(word(B11111111,B11111111));  
}

// Read the LCD panel buttons (from ButtonTest sketch).
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (adc_key_in < 50)   return btnRIGHT; 
 if (adc_key_in < 195)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 555)  return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}

// Display the connection combination.
void show_connection(String thistothat) {
  // thistothat is intended to look like "ab"
  lcd.setCursor(0, 0);
  lcd.print(thistothat);
}

// Set the relays to the desired pattern.
void set_relays(byte pattern) {
  // patten is the bits that need to be set to turn on the correct relays
  Wire.beginTransmission(ioaddress);
  Wire.write(~pattern);
  Wire.write(0);
  Wire.endTransmission();
  delay(read_delay);
}

// Read and check the resut from the AD converter.
// Print a 1 or 0 on the LCD panel.
// Return 1 for continuity, 0 for not.
int get_reading() {
  int result = 1;
  int sensorvalue = analogRead(1);
  if (sensorvalue > threshold) {
    result = 0;
  }
  lcd.setCursor(2, 0);
  lcd.print(result);
  delay(relay_delay);
  return result;
 }

// Analyze continuity readings for open, ok, or short,
// Parameters: 
// col tells where to print result on LCD. 
// c1 and c2 are the clip lead connections like "ab".
// r1 and r1 are whether there's conductivity in that direction.
// Returns 0 for open, 1 for OK, 2 for short.
int show_reading(int col, String c1, int r1, String c2, int r2) {
  lcd.setCursor(col, 0);
  lcd.print("    ");
  lcd.setCursor(col, 0);
  int result = 0;
  // Count directions of conductivity,
  if (r1 == 1) {
    lcd.print(c1); 
    result++;
  }
  if (r2 == 1) {
    lcd.print(c2);
    result++;
  }
  return result;
}

void read_analyze_report() {
  // clear the LCD screen
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");

  // Set all six relay patterns and read results.
  // Leads ab and ac
  show_connection("ab");
  set_relays(atob);
  int rab = get_reading();
  show_connection("ba");
  set_relays(btoa);
  int rba = get_reading();
  int resultab = show_reading(4, "ab", rab, "ba", rba);

  // Leads bc and cb
  show_connection("bc");
  set_relays(btoc);
  int rbc = get_reading();
  show_connection("cb");
  set_relays(ctob);
  int rcb = get_reading();
  int resultbc = show_reading(8, "bc", rbc, "cb", rcb);

  // Leads ac and ca
  show_connection("ac");
  set_relays(atoc);
  int rac = get_reading();
  show_connection("ca");
  set_relays(ctoa);
  int rca = get_reading();
  int resultac = show_reading(12, "ac", rac, "ca", rca);

  // Set up to report results.
  lcd.setCursor(0, 1);
  int shorted = 0;
  int opened = 0;
  String shortedstring = "";
  String openstring = "";

  // Check for shorts.
  if (resultab == 2 | resultac == 2 | resultbc == 2) {
    if (resultab == 2) {
      shorted++;
      shortedstring = shortedstring + "ab ";
    }
    if (resultac == 2) {
      shorted++;
      shortedstring = shortedstring + "ac ";
    }
    if (resultbc == 2) {
      shorted++;
      shortedstring = shortedstring + "bc ";
    }
  } 

  // Check for opens.
  if (resultab == 0 | resultac == 0 | resultbc == 0) {
    opened = 0;
    if (resultab == 0) {
      opened++;
      openstring = openstring + "ab ";
    }
    if (resultac == 0) {
      opened++;
      openstring = openstring + "ac ";
    }
    if (resultbc == 0) {
      opened++;
      openstring = openstring + "bc ";
    }
  }

  // The reading between emitter and collector will always
  // read as "open" so we want to see exactly one open. 
  // Shorts are bad. 
  if (opened != 1 | shorted != 0) {
    // Bad. Print findings. 
    lcd.print("Bad:");
    if (shorted != 0) {
      lcd.print("S:"+shortedstring);
    }
    if (opened != 0) {
      lcd.print("O:"+openstring);
    }
  } else {
    // Good. Find the base. 
    lcd.print("OK ");
    String base = "";
    String type = "";
    
    if (openstring == "ab ") {
      base = "c";
      if (rac == 1 && rbc == 1) {
        type = "PNP";
      } else {
        type = "NPN";
      }
      
    } else if (openstring == "ac ") {
      base = "b";
      if (rab == 1 && rcb == 1) {
        type = "PNP";
      } else {
        type = "NPN";
      }
      
    } else if (openstring == "bc ") {
      base = "a";
      if (rca == 1 && rba == 1) {
        type = "PNP";
      } else {
        type = "NPN";
      }
    }

    // Print the results on the LCD screen.
    lcd.print(type + " base:" + base);
  }
}

void loop() {
  // Read the lcd panel buttons.
  // If any button is pressed, start the analysis. 
  if (read_LCD_buttons() != 5) {
      read_analyze_report();
  }
}
