#ifndef cf_lib_websocket
#define cf_lib_websockek

#include <FreeRTOS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class cf_websocket
{
public:
    // constructor
    cf_websocket();

    // attributes
    AsyncWebServer server();
    AsyncWebSocket ws();
    AsyncEventSource events();
    // static const char* ssid = 'Berlin';
    // static const char* password = "carrotMolly";
    // IPAddress* myWiFi = &WiFi.localIP();
    std::string sWiFi;

    // methods
    void begin();
    // void createObj();
    void loop();
    void notifyClients();
    static void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
    static void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                 void *arg, uint8_t *data, size_t len);
    void initWebSocket();
    String processor(const String &var);

    // html
    const char index_html[6500] PROGMEM = R"rawliteral(
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
};

#endif