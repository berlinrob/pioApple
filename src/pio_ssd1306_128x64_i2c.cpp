/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10 // Number of snowflakes in the animation example

// #define LOGO_HEIGHT   16
// #define LOGO_WIDTH    16
// static const unsigned char PROGMEM logo_bmp[] =
// { B00000000, B11000000,
//   B00000001, B11000000,
//   B00000001, B11000000,
//   B00000011, B11100000,
//   B11110011, B11100000,
//   B11111110, B11111000,
//   B01111110, B11111111,
//   B00110011, B10011111,
//   B00011111, B11111100,
//   B00001101, B01110000,
//   B00011011, B10100000,
//   B00111111, B11100000,
//   B00111111, B11110000,
//   B01111100, B11110000,
//   B01110000, B01110000,
//   B00000000, B00110000 };

#define joyX 34  //analog x from 0~4095
#define joyY 35  //analog y from 0~4095
#define joySW 19 //digital switch on joystick

int analogreadX = 0;
int analogreadY = 0;
int newX = 0;
int newY = 0;
char cstr[16];
//char localWiFi[16];
IPAddress myWiFi;
std::string sWiFi;

int previousX = 0;
int previousY = 0;

int screenTransitionIncrement = 0;
bool moveMenu = false;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
static const unsigned long MENU_DELAY_INTERVAL = 2000; // ms

// Replace with your network credentials
const char *ssid = "Berlin";
const char *password = "carrotMolly";

bool ledState = 0;
const int ledPin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

AsyncEventSource events("/events");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>ESP WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Output - GPIO 2</h2>
      <p class="state">state: <span id="state">%STATE%</span></p>
      <p><button id="button" class="button">Toggle</button></p>
      <p class="x">X: <span id="x">%X%</span></p>
      <p class="y">Y: <span id="y">%Y%</span></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);

if (!!window.EventSource) {
  var source = new EventSource('/events');

  source.addEventListener('open', function(e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);

  source.addEventListener('message', function(e) {
    console.log("message", e.data);
  }, false);

  source.addEventListener('jsX', function(e) {
    document.getElementById('x').innerHTML = e.data;
  }, false);

  
  source.addEventListener('jsY', function(e) {
    document.getElementById('y').innerHTML = e.data;
  }, false);}  
  
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var state;
    if (event.data == "1"){
      state = "ON";
    }
    else{
      state = "OFF";
    }
    document.getElementById('state').innerHTML = state;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
  }
  function toggle(){
    websocket.send('toggle');
  }
</script>
</body>
</html>
)rawliteral";

void notifyClients()
{
  ws.textAll(String(ledState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    if (strcmp((char *)data, "toggle") == 0)
    {
      ledState = !ledState;
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String &var)
{
  Serial.println(var);
  if (var == "STATE")
  {
    if (ledState)
    {
      return "ON";
    }
    else
    {
      return "OFF";
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.flush();

  Serial.println("about to try to connect to wifi....");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    printf("Connecting to WiFi..");
    // Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());
  //localWiFi = WiFi.localIP().toString();
  sWiFi = WiFi.localIP();
  printf("Local WiFi: %s", sWiFi);

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  //   event source setup
  events.onConnect([](AsyncEventSourceClient *client) {
    if (client->lastId())
    {
      Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
    }
    //send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 1000);
  });
  //HTTP Basic authentication
  events.setAuthentication("user", "pass");
  server.addHandler(&events);

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
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
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  // testdrawline();      // Draw many lines

  // testdrawrect();      // Draw rectangles (outlines)

  // testfillrect();      // Draw rectangles (filled)

  // testdrawcircle();    // Draw circles (outlines)

  // testfillcircle();    // Draw circles (filled)

  // testdrawroundrect(); // Draw rounded rectangles (outlines)

  // testfillroundrect(); // Draw rounded rectangles (filled)

  // testdrawtriangle();  // Draw triangles (outlines)

  // testfilltriangle();  // Draw triangles (filled)

  // testdrawchar();      // Draw characters of the default font

  // testdrawstyles();    // Draw 'stylized' characters

  // testscrolltext();    // Draw scrolling text

  // testdrawbitmap();    // Draw a small bitmap image

  // // Invert and restore display, pausing in-between
  // display.invertDisplay(true);
  // delay(1000);
  // display.invertDisplay(false);
  // delay(1000);

  // testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
}

void loop()
{
  ws.cleanupClients();
  display.clearDisplay();

  previousX = newX;
  previousY = newY;

  analogreadX = analogRead(joyX);
  analogreadY = analogRead(joyY);
  newX = map(analogreadX, 0, 4095, 0, 127);
  newY = map(analogreadY, 0, 4095, 0, 63);

  display.fillRect(newX, newY, 5, 5, SSD1306_WHITE);
  // Serial.print(analogreadX);
  // Serial.print("-x-");
  // Serial.print(analogreadX, DEC);
  // Serial.print(", ");
  // Serial.print(analogreadY);
  // Serial.print("-y-");
  // Serial.println(analogreadY, DEC);
  // Serial.print(newX, DEC);
  // Serial.print(", ");
  // Serial.println(newY, DEC);

  //delay(50);

  display.display();

  // test for github

  printf("%d -- %d : %s\n", newX, newY, WiFi.localIP());

  // for event sourced
  //if(eventTriggered){ // your logic here
  //send event "myevent"
  // events.send("1","jsX",millis());
  // events.send("2","jsY",millis());

  //if(previousX - newX > 1 || previousX - newX < -1){
  events.send(itoa(newX, cstr, 10), "jsX", millis());
  events.send(itoa(newY, cstr, 10), "jsY", millis());
  //}
  //}

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

// void testdrawline() {
//   int16_t i;

//   display.clearDisplay(); // Clear display buffer

//   for(i=0; i<display.width(); i+=4) {
//     display.drawLine(0, 0, i, display.height()-1, SSD1306_WHITE);
//     display.display(); // Update screen with each newly-drawn line
//     delay(1);
//   }
//   for(i=0; i<display.height(); i+=4) {
//     display.drawLine(0, 0, display.width()-1, i, SSD1306_WHITE);
//     display.display();
//     delay(1);
//   }
//   delay(250);

//   display.clearDisplay();

//   for(i=0; i<display.width(); i+=4) {
//     display.drawLine(0, display.height()-1, i, 0, SSD1306_WHITE);
//     display.display();
//     delay(1);
//   }
//   for(i=display.height()-1; i>=0; i-=4) {
//     display.drawLine(0, display.height()-1, display.width()-1, i, SSD1306_WHITE);
//     display.display();
//     delay(1);
//   }
//   delay(250);

//   display.clearDisplay();

//   for(i=display.width()-1; i>=0; i-=4) {
//     display.drawLine(display.width()-1, display.height()-1, i, 0, SSD1306_WHITE);
//     display.display();
//     delay(1);
//   }
//   for(i=display.height()-1; i>=0; i-=4) {
//     display.drawLine(display.width()-1, display.height()-1, 0, i, SSD1306_WHITE);
//     display.display();
//     delay(1);
//   }
//   delay(250);

//   display.clearDisplay();

//   for(i=0; i<display.height(); i+=4) {
//     display.drawLine(display.width()-1, 0, 0, i, SSD1306_WHITE);
//     display.display();
//     delay(1);
//   }
//   for(i=0; i<display.width(); i+=4) {
//     display.drawLine(display.width()-1, 0, i, display.height()-1, SSD1306_WHITE);
//     display.display();
//     delay(1);
//   }

//   delay(2000); // Pause for 2 seconds
// }

// void testdrawrect(void) {
//   display.clearDisplay();

//   for(int16_t i=0; i<display.height()/2; i+=2) {
//     display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
//     display.display(); // Update screen with each newly-drawn rectangle
//     delay(1);
//   }

//   delay(2000);
// }

// void testfillrect(void) {
//   display.clearDisplay();

//   for(int16_t i=0; i<display.height()/2; i+=3) {
//     // The INVERSE color is used so rectangles alternate white/black
//     display.fillRect(i, i, display.width()-i*2, display.height()-i*2, SSD1306_INVERSE);
//     display.display(); // Update screen with each newly-drawn rectangle
//     delay(1);
//   }

//   delay(2000);
// }

// void testdrawcircle(void) {
//   display.clearDisplay();

//   for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
//     display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
//     display.display();
//     delay(1);
//   }

//   delay(2000);
// }

// void testfillcircle(void) {
//   display.clearDisplay();

//   for(int16_t i=max(display.width(),display.height())/2; i>0; i-=3) {
//     // The INVERSE color is used so circles alternate white/black
//     display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
//     display.display(); // Update screen with each newly-drawn circle
//     delay(1);
//   }

//   delay(2000);
// }

// void testdrawroundrect(void) {
//   display.clearDisplay();

//   for(int16_t i=0; i<display.height()/2-2; i+=2) {
//     display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
//       display.height()/4, SSD1306_WHITE);
//     display.display();
//     delay(1);
//   }

//   delay(2000);
// }

// void testfillroundrect(void) {
//   display.clearDisplay();

//   for(int16_t i=0; i<display.height()/2-2; i+=2) {
//     // The INVERSE color is used so round-rects alternate white/black
//     display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
//       display.height()/4, SSD1306_INVERSE);
//     display.display();
//     delay(1);
//   }

//   delay(2000);
// }

// void testdrawtriangle(void) {
//   display.clearDisplay();

//   for(int16_t i=0; i<max(display.width(),display.height())/2; i+=5) {
//     display.drawTriangle(
//       display.width()/2  , display.height()/2-i,
//       display.width()/2-i, display.height()/2+i,
//       display.width()/2+i, display.height()/2+i, SSD1306_WHITE);
//     display.display();
//     delay(1);
//   }

//   delay(2000);
// }

// void testfilltriangle(void) {
//   display.clearDisplay();

//   for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
//     // The INVERSE color is used so triangles alternate white/black
//     display.fillTriangle(
//       display.width()/2  , display.height()/2-i,
//       display.width()/2-i, display.height()/2+i,
//       display.width()/2+i, display.height()/2+i, SSD1306_INVERSE);
//     display.display();
//     delay(1);
//   }

//   delay(2000);
// }

// void testdrawchar(void) {
//   display.clearDisplay();

//   display.setTextSize(1);      // Normal 1:1 pixel scale
//   display.setTextColor(SSD1306_WHITE); // Draw white text
//   display.setCursor(0, 0);     // Start at top-left corner
//   display.cp437(true);         // Use full 256 char 'Code Page 437' font

//   // Not all the characters will fit on the display. This is normal.
//   // Library will draw what it can and the rest will be clipped.
//   for(int16_t i=0; i<256; i++) {
//     if(i == '\n') display.write(' ');
//     else          display.write(i);
//   }

//   display.display();
//   delay(2000);
// }

// void testdrawstyles(void) {
//   display.clearDisplay();

//   display.setTextSize(1);             // Normal 1:1 pixel scale
//   display.setTextColor(SSD1306_WHITE);        // Draw white text
//   display.setCursor(0,0);             // Start at top-left corner
//   display.println(F("Hello, world!"));

//   display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
//   display.println(3.141592);

//   display.setTextSize(2);             // Draw 2X-scale text
//   display.setTextColor(SSD1306_WHITE);
//   display.print(F("0x")); display.println(0xDEADBEEF, HEX);

//   display.display();
//   delay(2000);
// }

// void testscrolltext(void) {
//   display.clearDisplay();

//   display.setTextSize(2); // Draw 2X-scale text
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(10, 0);
//   display.println(F("scroll"));
//   display.display();      // Show initial text
//   delay(100);

//   // Scroll in various directions, pausing in-between:
//   display.startscrollright(0x00, 0x0F);
//   delay(2000);
//   display.stopscroll();
//   delay(1000);
//   display.startscrollleft(0x00, 0x0F);
//   delay(2000);
//   display.stopscroll();
//   delay(1000);
//   display.startscrolldiagright(0x00, 0x07);
//   delay(2000);
//   display.startscrolldiagleft(0x00, 0x07);
//   delay(2000);
//   display.stopscroll();
//   delay(1000);
// }

// void testdrawbitmap(void) {
//   display.clearDisplay();

//   display.drawBitmap(
//     (display.width()  - LOGO_WIDTH ) / 2,
//     (display.height() - LOGO_HEIGHT) / 2,
//     logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
//   display.display();
//   delay(1000);
// }

// #define XPOS   0 // Indexes into the 'icons' array in function below
// #define YPOS   1
// #define DELTAY 2

// void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
//   int8_t f, icons[NUMFLAKES][3];

//   // Initialize 'snowflake' positions
//   for(f=0; f< NUMFLAKES; f++) {
//     icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
//     icons[f][YPOS]   = -LOGO_HEIGHT;
//     icons[f][DELTAY] = random(1, 6);
//     Serial.print(F("x: "));
//     Serial.print(icons[f][XPOS], DEC);
//     Serial.print(F(" y: "));
//     Serial.print(icons[f][YPOS], DEC);
//     Serial.print(F(" dy: "));
//     Serial.println(icons[f][DELTAY], DEC);
//   }

//   for(;;) { // Loop forever...
//     display.clearDisplay(); // Clear the display buffer

//     // Draw each snowflake:
//     for(f=0; f< NUMFLAKES; f++) {
//       display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
//     }

//     display.display(); // Show the display buffer on the screen
//     delay(200);        // Pause for 1/10 second

//     // Then update coordinates of each flake...
//     for(f=0; f< NUMFLAKES; f++) {
//       icons[f][YPOS] += icons[f][DELTAY];
//       // If snowflake is off the bottom of the screen...
//       if (icons[f][YPOS] >= display.height()) {
//         // Reinitialize to a random position, just off the top
//         icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
//         icons[f][YPOS]   = -LOGO_HEIGHT;
//         icons[f][DELTAY] = random(1, 6);
//       }
//     }
//    }
// }
