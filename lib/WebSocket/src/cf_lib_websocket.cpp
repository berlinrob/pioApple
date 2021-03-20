#include "cf_lib_websocket.h"
#include <FreeRTOS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

    //AsyncWebServer server(80);
    //AsyncWebSocket ws("/ws");
    //AsyncEventSource events("/events");

cf_websocket::cf_websocket(){
    //AsyncWebServer server_temp(80);
    //cf_websocket::server = server_temp;
    // // Create AsyncWebServer object on port 80
    // cf_websocket::server = AsyncWebServer(80);
    // cf_websocket::ws = AsyncWebSocket("/ws");

    // cf_websocket::events = AsyncEventSource("/events");

    // cf_websocket::createObj();
    server = AsyncWebServer(80);
    // cf_websocket::server() = AsyncWebServer(80);
    ws = AsyncWebSocket("/ws");
    // cf_websocket::ws() = AsyncWebSocket("/ws");
    events = AsyncEventSource("/events");
    // cf_websocket::events() = AsyncEventSource("/events");
}

// void cf_websocket::createObj(){
//     cf_websocket::server() = AsyncWebServer(80);
// }


void cf_websocket::notifyClients()
{
    //ws.textAll(String(ledState));
}

void cf_websocket::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;
        if (strcmp((char *)data, "toggle") == 0)
        {
            //ledState = !ledState;
            //notifyClients();
        }
    }
}

void cf_websocket::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
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

void cf_websocket::initWebSocket()
{   
    ws.onEvent(onEvent);
    server.addHandler(&cf_websocket::ws);
}

String cf_websocket::processor(const String &var)
{
    Serial.println(var);
    if (var == "STATE")
    {
        // if (ledState)
        // {
        //   return "ON";
        // }
        // else
        // {
        //   return "OFF";
        // }
    }
    return "OFF";
}

void cf_websocket::begin()
{
    Serial.println("about to try to connect to wifi....");

    // Connect to Wi-Fi
    // WiFi.begin(cf_websocket::ssid, cf_websocket::password);
    WiFi.begin("Berlin", "carrotMolly");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        printf("Connecting to WiFi from lib..");
        // Serial.println("Connecting to WiFi..");
    }


    // Print ESP Local IP Address
    Serial.println(WiFi.localIP());
    //cf_websocket::localWiFi = *WiFi.localIP().toString();
    cf_websocket::sWiFi = WiFi.localIP().toString().c_str();
    printf("from library ==== Local WiFi: %s", cf_websocket::sWiFi);
    // printf("from library ==== Local WiFi: %s", cf_websocket::localWiFi);

    // cf_websocket::initWebSocket();
    cf_websocket::initWebSocket();
    // this->initWebSocket();

    //const char* myChar = "off";


    // Route for root / web page
    //cf_websocket::initWebSocket();
    cf_websocket::server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", "<html></html>", cf_websocket::processor);
    });
    // cf_websocket::server().on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    //     request->send_P(200, "text/html", cf_websocket::index_html, cf_websocket::processor());
    // });

    // Start server
    cf_websocket::server.begin();
    // cf_websocket::server.begin();

    //   event source setup
    cf_websocket::events.onConnect([](AsyncEventSourceClient *client) {
        if (client->lastId())
        {
            Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
        }
        //send event with message "hello!", id current millis
        // and set reconnect delay to 1 second
        client->send("hello!", NULL, millis(), 1000);
    });
    //HTTP Basic authentication
    cf_websocket::events.setAuthentication("user", "pass");
    cf_websocket::server.addHandler(&cf_websocket::events);
}

void cf_websocket::_loop(){
    // cf_websocket::ws.cleanupClients();
//   cf_websocket::events.send(itoa(newX, cstr, 10), "jsX", millis());
//   cf_websocket::events.send(itoa(newY, cstr, 10), "jsY", millis());
    //printf("%s/n", WiFi.localIP());
}