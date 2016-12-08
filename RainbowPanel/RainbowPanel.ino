/* This example shows how to display a moving two-dimensional
 * rainbow pattern on an APA102-based LED panel.
 *
 * Be sure to edit the ledPanelWidth and ledPanelHeight variables
 * below to match your LED panel.  For the 8x32 panel from Pololu,
 * ledPanelWidth should be 8 and ledPanelHeight should be 32. */

/* This example is meant for controlling large numbers of LEDs,
 * so it requires the FastGPIO library.  If you cannot use the
 * FastGPIO library, you can comment out the two lines below and
 * the example will still work, but it will be slow. */
//#include <FastGPIO.h>
//#define APA102_USE_FAST_GPIO

#include <APA102.h>
#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

// COMMON SETTINGS
// ----------------------------------------------------------------------------------------------
// These settings are used in both SW UART, HW UART and SPI mode
// ----------------------------------------------------------------------------------------------
#define BUFSIZE                        16384   // Size of the read buffer for incoming data
#define VERBOSE_MODE                   false  // If set to 'true' enables debug output


// SOFTWARE UART SETTINGS
// ----------------------------------------------------------------------------------------------
// The following macros declare the pins that will be used for 'SW' serial.
// You should use this option if you are connecting the UART Friend to an UNO
// ----------------------------------------------------------------------------------------------
#define BLUEFRUIT_SWUART_RXD_PIN       9    // Required for software serial!
#define BLUEFRUIT_SWUART_TXD_PIN       10   // Required for software serial!
#define BLUEFRUIT_UART_CTS_PIN         11   // Required for software serial!
#define BLUEFRUIT_UART_RTS_PIN         -1   // Optional, set to -1 if unused


// HARDWARE UART SETTINGS
// ----------------------------------------------------------------------------------------------
// The following macros declare the HW serial port you are using. Uncomment
// this line if you are connecting the BLE to Leonardo/Micro or Flora
// ----------------------------------------------------------------------------------------------
#ifdef Serial1    // this makes it not complain on compilation if there's no Serial1
  #define BLUEFRUIT_HWSERIAL_NAME      Serial1
#endif


// SHARED UART SETTINGS
// ----------------------------------------------------------------------------------------------
// The following sets the optional Mode pin, its recommended but not required
// ----------------------------------------------------------------------------------------------
#define BLUEFRUIT_UART_MODE_PIN        12    // Set to -1 if unused


// SHARED SPI SETTINGS
// ----------------------------------------------------------------------------------------------
// The following macros declare the pins to use for HW and SW SPI communication.
// SCK, MISO and MOSI should be connected to the HW SPI pins on the Uno when
// using HW SPI.  This should be used with nRF51822 based Bluefruit LE modules
// that use SPI (Bluefruit LE SPI Friend).
// ----------------------------------------------------------------------------------------------
#define BLUEFRUIT_SPI_CS               8
#define BLUEFRUIT_SPI_IRQ              7
#define BLUEFRUIT_SPI_RST              4    // Optional but recommended, set to -1 if unused

// SOFTWARE SPI SETTINGS
// ----------------------------------------------------------------------------------------------
// The following macros declare the pins to use for SW SPI communication.
// This should be used with nRF51822 based Bluefruit LE modules that use SPI
// (Bluefruit LE SPI Friend).
// ----------------------------------------------------------------------------------------------
#define BLUEFRUIT_SPI_SCK              13
#define BLUEFRUIT_SPI_MISO             12
#define BLUEFRUIT_SPI_MOSI             11

/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}


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

/* Converts a color from HSV to RGB.
 * h is hue, as a number between 0 and 360.
 * s is the saturation, as a number between 0 and 255.
 * v is the value, as a number between 0 and 255. */
rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return (rgb_color){r, g, b};
}

void setup()
{
  // Open serial connection.
  Serial.begin(115200);
  pinMode(dataPin, OUTPUT);
  //Serial.write('7');
  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
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

  Serial.println(F("******************************"));

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

  Serial.println(F("******************************"));

  
}

/* Returns a reference to the color at the specified coordinates,
 * where x specifies the column and y specifies the row.  For
 * efficiency, this function does not check the values of x and
 * y, so x must be less than ledPanelWidth and y must be less
 * than ledPanelHeight. */
//rgb_color & color_at(uint8_t x, uint8_t y)
//{
//  /* The LEDs in the panel are arranged in a serpentine layout,
//   * so if we are in an even-numbered column then flip the x
//   * coordinate. */
//  if (!(y & 1)) { x = ledPanelWidth - 1 - x; }
//  return colors[(ledPanelWidth * y) + x];
//}

void loop()
{
  uint8_t time = millis() >> 2;
  // If there's a new connection, flash all green
  if(Serial.available() || ble.available()) {
    // Return rainbow if string is not empty
    String c = "";
    if (Serial.available()){
      c = Serial.readString();
    } else if (ble.available()) {
      c = ble.readString();
    }
    if (c.length() != 0) {
      if (c[0] == '0') {
        str0 = c.substring(1,61);
      } else if (c[0] == '1') {
        str1 = c.substring(1,61);
      } else if (c[0] == '2') {
        str2 = c.substring(1,51);
      }
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
      if (c[0] == '2') {
        while (true) {
          if (str2[49] == '1') {
            ledStrip.write(colors, ledCount, brightness);
            Serial.println(c[ledCount]);
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
      // if the string is empty, then flash green
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
      // No connection, first time, flash red
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
