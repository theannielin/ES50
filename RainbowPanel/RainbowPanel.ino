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

// Define which pins to use.
const uint8_t dataPin = 11;
const uint8_t clockPin = 12;

// Create an object for writing to the LED strip.
APA102<dataPin, clockPin> ledStrip;

// Set the size of the LED panel.
const uint8_t ledPanelWidth = 13;
const uint8_t ledPanelHeight = 13;
const uint16_t ledCount = ledPanelWidth * ledPanelHeight;

// Create a buffer for holding the colors (3 bytes per color).
rgb_color colors[ledCount];

// Set the brightness to use (the maximum is 31).
const uint8_t brightness = 1;

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
  if(Serial.available()) {
    // Return rainbow if string is not empty
    String c = Serial.readString();
    if (c.length() != 0) {
      for (uint8_t x = 0; x < ledPanelWidth; x++)
      {
        for (uint8_t y = 0; y < ledPanelHeight; y++)
        {
          //Serial.print(c.length());
          //Serial.print(c[ledPanelWidth*x+y]);
          if (c[ledPanelWidth*x+y]=='0') {
            colors[x*13+y] = (rgb_color){0,0,0}; // off
          }
          else if (c[ledPanelWidth*x+y]=='1') {
            colors[x*13+y] = (rgb_color){0,255,0}; // green
          }
          else if (c[ledPanelWidth*x+y]=='2') {
            colors[x*13+y] = (rgb_color){0,0,255}; // blue
          }
          else if (c[ledPanelWidth*x+y]=='3') {
            colors[x*13+y] = (rgb_color){255,0,0}; // red
          }
          else if (c[ledPanelWidth*x+y]=='4') {
            colors[x*13+y] = (rgb_color){255,255,0}; // yellow
          }
          else if (c[ledPanelWidth*x+y]=='5') {
            colors[x*13+y] = (rgb_color){148,0,211}; // purple
          }
          else if (c[ledPanelWidth*x+y]=='6') {
            colors[x*13+y] = (rgb_color){255,140,0}; // orange
          }
        }
        // colors[1*13+0] = (rgb_color){255,0,0};
      }
      while (true) {
        ledStrip.write(colors, ledCount, brightness);
        //Serial.println(c[ledPanelWidth*1+2]);
        if (Serial.available())
           break;
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
        if (Serial.available())
           break;
      }
    }
  } else {
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
