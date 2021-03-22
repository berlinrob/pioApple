#include <iostream>
//#include <WiFi.h>

//#include <SPI.h>
//#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// custom libraries
#include "cf_lib_JoyStick.h"
// #include "cf_lib_oled128x64.h"
// #include "cf_lib_websocket.h"

// display
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
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

int myOffset = 0;

// bool moveMenu = false;
int menuIndex = 0;
const char *myMenu[] = {"ONE", "TWO", "THREE", "FOUR"};

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
static const unsigned long MENU_DELAY_INTERVAL = 500; // ms

bool ledState = 0;
const int ledPin = 2;

void setup()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.begin(115200);
    delay(1000);
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.display();
  delay(250);
  // Clear the buffer
  display.clearDisplay();
  display.display();
  delay(500);
}

void loop()
{
  // ws.cleanupClients();
  display.clearDisplay();

  previousX = newX;
  previousY = newY;

  myJoyStick.setMapX(map(myJoyStick.getAnalogX(), 0, 4095, 0, 127));
  myJoyStick.setMapY(map(myJoyStick.getAnalogY(), 0, 4095, 0, 63));

  display.fillRect(myJoyStick.mapX, myJoyStick.mapY, 5, 5, SSD1306_WHITE);

  display.setTextSize(3);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner

  // display.printf("X: %d\n", myJoyStick.mapX);
  // display.printf("Y: %d\n", myJoyStick.mapY);
  // display.printf("previousMillis: %lx\n", previousMillis);
  display.printf("%s\n", myMenu[menuIndex]);

  display.display();

  // demo - menu transition
  if (myJoyStick.mapY < 5 || myJoyStick.mapY > 58)
  {
    if (previousMillis == 0)
    {
      previousMillis = millis();
    }
    else if (millis() - previousMillis > MENU_DELAY_INTERVAL)
    {
      if (myJoyStick.mapY < 5)
      {
        myOffset = 0;
        menuIndex == 0 ? menuIndex = 3 : menuIndex--;
      }
      else
      {
        myOffset = 60;
        menuIndex == 3 ? menuIndex = 0 : menuIndex++;
      }

      for (int i = 0; i <= 12; i++)
      {
        display.clearDisplay();

        // display.drawLine(0, abs(myOffset - 5 * i), SCREEN_WIDTH, abs(myOffset - 5 * i), SSD1306_WHITE);

        if (myOffset == 0)
        {

          display.setCursor(0, abs(-5 * i) - SCREEN_HEIGHT); // Start at top-left corner
          // display.printf("X: %d\n", myJoyStick.mapX);
          // display.printf("Y: %d\n", myJoyStick.mapY);
          // display.printf("previousMillis: %lx\n", previousMillis);
          display.printf("%s\n", myMenu[menuIndex]);
        }
        else
        {

          display.setCursor(0, -5 * i); // Start at top-left corner
          // display.printf("X: %d\n", myJoyStick.mapX);
          // display.printf("Y: %d\n", myJoyStick.mapY);
          // display.printf("previousMillis: %lx\n", previousMillis);
          display.printf("%s\n", myMenu[menuIndex]);
        }

        display.setCursor(0, abs(myOffset - 5 * i)); // Start at top-left corner
        // display.printf("X: %d\n", myJoyStick.mapX);
        // display.printf("Y: %d\n", myJoyStick.mapY);
        // display.printf("previousMillis: %lx\n", previousMillis);
        display.printf("%s\n", myMenu[menuIndex]);
        display.display();
      }
      previousMillis = 0;
    }
  }
  else
  {
    previousMillis = 0;
  }
}