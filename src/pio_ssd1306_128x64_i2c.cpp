#include <iostream>
//#include <WiFi.h>

//#include <SPI.h>
//#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// custom libraries
#include "cf_lib_JoyStick.h"
// #include "cf_lib_oled128x64.h"
// #include "cf_lib_websocket.h"

// display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// joystick
#define joyX 34  //analog x from 0~4095
#define joyY 35  //analog y from 0~4095
#define joySW 19 //digital switch on joystick

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
cf_joystick myJoyStick(joyX, joyY, joySW);

int newX = 0;
int newY = 0;
int previousX = 0;
int previousY = 0;

int screenTransitionIncrement = 0;
bool moveMenu = false;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
static const unsigned long MENU_DELAY_INTERVAL = 2000; // ms

bool ledState = 0;
const int ledPin = 2;

void setup()
{
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
}

void loop()
{
  // ws.cleanupClients();
  display.clearDisplay();

  previousX = newX;
  previousY = newY;

  newX = map(myJoyStick.getAnalogX(), 0, 4095, 0, 127);
  newY = map(myJoyStick.getAnalogY(), 0, 4095, 0, 63);

  display.fillRect(newX, newY, 5, 5, SSD1306_WHITE);

  display.display();

  myJoyStick.begin();
  myJoyStick.loop();

  // demo display transition moving down
  if (newY == 0)
  {
    if (previousMillis == 0)
    {
      previousMillis = millis();
    }
    else if (millis() - previousMillis > MENU_DELAY_INTERVAL)
    {
      screenTransitionIncrement = 0;

      while (screenTransitionIncrement < 10)
      {
        display.clearDisplay();
        display.drawLine(0, screenTransitionIncrement * 6, SCREEN_WIDTH, screenTransitionIncrement * 6, SSD1306_WHITE);
        screenTransitionIncrement++;
        display.display();
      }

      previousMillis = 0;
    }
  }

  // demo display transition moving up
  if (newY == 58)
  {
    if (previousMillis == 0)
    {
      previousMillis = millis();
    }
    else if (millis() - previousMillis > MENU_DELAY_INTERVAL)
    {
      screenTransitionIncrement = 58;

      while (screenTransitionIncrement > 0)
      {
        display.clearDisplay();
        display.drawLine(0, screenTransitionIncrement, SCREEN_WIDTH, screenTransitionIncrement, SSD1306_WHITE);
        screenTransitionIncrement -= 5;
        display.display();
      }

      previousMillis = 0;
    }
  }
}