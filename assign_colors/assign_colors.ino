/* Arduino Code for ES50 Fall 2016 Team 10: Customizable LED Jacket
 * Authors: Hailey James, Melissa ChaeHyun Lee, Annie Lin, Stephen Slater
 */

#include <APA102.h>
#include "Adafruit_BluefruitLE_SPI.h"

#define BUFSIZE                        16384   // Size of the read buffer for incoming data
#define VERBOSE_MODE                   false  // If set to 'true' enables debug output
#define BLUEFRUIT_SPI_CS               8
#define BLUEFRUIT_SPI_IRQ              7
#define BLUEFRUIT_SPI_RST              4    // Optional but recommended, set to -1 if unused
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

/* Hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// Define which pins to use.
const uint8_t dataPin = 5;
const uint8_t clockPin = 6;

// Create an object for writing to the LED strip.
APA102<dataPin, clockPin> ledStrip;

// Set the size of the LED panel.
const uint8_t ledPanelWidth = 13;
const uint8_t ledPanelHeight = 13;
const uint16_t ledCount = ledPanelWidth * ledPanelHeight;

// Create a buffer for holding the colors (3 bytes per color).
rgb_color colors[ledCount];

// Create a buffer for holding the colors (3 bytes per color).
rgb_color not_colors[ledCount];

// Set the brightness to use (the maximum is 31).
const uint8_t brightness = 1;

String str0;
String str1;
String str2;
String finalStr;

void setup()
{
  // Open serial connection.
  Serial.begin(115200);
  pinMode(dataPin, OUTPUT);

  ble.begin(VERBOSE_MODE);
  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA); 
}

void loop()
{
  uint8_t time = millis() >> 2;
  if(Serial.available() || ble.available()) {
    String c = "";
    if (Serial.available()){
      c = Serial.readString();
    } else if (ble.available()) {
      c = ble.readString();
    }
    
    // Assign strings 0, 1, and 2, used to separate the strings because of
    // character length restrictions
    if (c.length() != 0) {
      if (c[0] == '0') {
        str0 = c.substring(1,61);
      } else if (c[0] == '1') {
        str1 = c.substring(1,61);
      } else if (c[0] == '2') {
        str2 = c.substring(1,51);
      }

      // Concatenate 3 string inputs 
      // then interpret string and assign colors 
      finalStr = str0 + str1 + str2;
      for (uint8_t x = 0; x < ledPanelWidth; x++)
      {
        for (uint8_t y = 0; y < ledPanelHeight; y++)
        {
          if (finalStr[ledPanelWidth*x+y]=='0') {
            colors[x*13+y] = (rgb_color){0,0,0}; // off
          }
          else if (finalStr[ledPanelWidth*x+y]=='1') {
            colors[x*13+y] = (rgb_color){255,0,0}; // red
          }
          else if (finalStr[ledPanelWidth*x+y]=='2') {
            colors[x*13+y] = (rgb_color){153,76,0}; // orange
          }
          else if (finalStr[ledPanelWidth*x+y]=='3') {
            colors[x*13+y] = (rgb_color){255,255,0}; // yellow
          }
          else if (finalStr[ledPanelWidth*x+y]=='4') {
            colors[x*13+y] = (rgb_color){0,255,0}; // green
          }
          else if (finalStr[ledPanelWidth*x+y]=='5') {
            colors[x*13+y] = (rgb_color){0,0,255}; // blue
          }
          else if (finalStr[ledPanelWidth*x+y]=='6') {
            colors[x*13+y] = (rgb_color){148,0,211}; // purple
          }
          not_colors[x*13+y] = (rgb_color){0,0,0}; 
        }
      }

      // Flashing lights on or off feature
      if (c[0] == '2') {
        while (true) {
          if (str2[49] == '1') {
            ledStrip.write(colors, ledCount, brightness);
            delay(500);
            ledStrip.write(not_colors, ledCount, brightness);
            delay(500);
          }
          else {
            ledStrip.write(colors, ledCount, brightness);
          }
          if (Serial.available() || ble.available())
             break;
        }
      }
    }
    else {
      // If the string is empty, then grid becomes all green
      for (uint8_t x = 0; x < ledPanelWidth; x++)
      {
        for (uint8_t y = 0; y < ledPanelHeight; y++)
        {
          colors[x*13+y] = (rgb_color){0,255,0};
        }
      }
      while (true) {
        ledStrip.write(colors, ledCount, brightness);
        if (Serial.available() || ble.available())
           break;
      }
    }
  } 
  else {
      // Grid turns red if connected but no string has been sent yet
      for (uint8_t x = 0; x < ledPanelWidth; x++)
        {
          for (uint8_t y = 0; y < ledPanelHeight; y++)
          {
            colors[x*13+y] = (rgb_color){255,0,0};
          }
        }
      ledStrip.write(colors, ledCount, brightness);
   }
}
