#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <NTP.h>
#include <WiFi.h>

AsyncWebServer server(80);

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const int WIFI_CHANNEL = 6;  // Speeds up the connection in Wokwi
WiFiUDP wifiUdp;
NTP ntp(wifiUdp);

void printBoxStart() {
  Serial.print("+");
  for (int ii = 0; ii < (80 - 2); ii++) {
    Serial.print("-");
  }
  Serial.println("+");
}

void printBoxEnd() {
  Serial.print("+");
  for (int ii = 0; ii < (80 - 2); ii++) {
    Serial.print("-");
  }
  Serial.print("+");
}

void printLine(char* data, size_t len) {
  Serial.print("|");
  for (int ii = 0; ii < len; ii++) {
    Serial.print(data[ii]);
  }
  Serial.println("|");
}
String one[7] = {"   #   ", "  ##   ", " # #   ", "   #   ",
                 "   #   ", "   #   ", " ##### "};
String two[7] = {" ##### ", "#     #", "      #", " ##### ",
                 "#      ", "#      ", "#######"};
String three[7] = {" ##### ", "#     #", "      #", " ##### ",
                   "      #", "#     #", " ##### "};
String four[7] = {"#      ", "#    # ", "#    # ", "#    # ",
                  "#######", "     # ", "     # "};
String five[7] = {"#######", "#      ", "#      ", "###### ",
                  "      #", "#     #", " ##### "};
String six[7] = {" ##### ", "#     #", "#      ", "###### ",
                 "#     #", "#     #", " ##### "};
String seven[7] = {"#######", "#    # ", "    #  ", "   #   ",
                   "  #    ", "  #    ", "  #    "};

String eight[7] = {" ##### ", "#     #", "#     #", " ##### ",
                   "#     #", "#     #", " ##### "};

String nine[7] = {" ##### ", "#     #", "#     #", " ######",
                  "      #", "#     #", " ##### "};
String zero[7] = {"  ###  ", " #   # ", "#     #", "#     #",
                  "#     #", " #   # ", "  ###  "};
String colon[7] = {" # ", "###", " # ", "   ", " # ", "###", " # "};

void printScreen(JsonDocument* doc) {
  Serial.print("\n");
  printBoxStart();
  for (int ii = 0; ii < (24 - 2); ii++) {
    char* line = new char[78]();
    for (int qq = 0; qq < (80 - 2); qq++) {
      line[qq] = ' ';
    }
    printLine(line, 78);
  }
  printBoxEnd();
  Serial.print("\r");
}

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}

void handleRoot(AsyncWebServerRequest* request) {
  request->send(200, "text/plain", "hello");
}

void handleData(AsyncWebServerRequest* request) {}

void handlePostData(AsyncWebServerRequest* request,
                    uint8_t* data,
                    size_t len,
                    size_t index,
                    size_t total) {
  if (len > 0) {
    JsonDocument* doc = new JsonDocument();
    auto err = deserializeJson(*doc, data, len);

    if (err == DeserializationError::Ok) {
      printScreen(doc);
      request->send(200, "text/plain", "recived");
      delete doc;
    } else {
      request->send(400, "text/plain", "invalid json");
    }
  } else {
    request->send(200, "text/plain", "no body");
  }
}

void setup() {
  Serial.begin(9600);
  Serial.print("Connecting to WiFi... ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password, WIFI_CHANNEL);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }
  Serial.println(" Connected!");

  ntp.ruleDST("PDT", Second, Sun, Mar, 2, 60 * -7);  // UTC-7
  ntp.ruleSTD("PST", First, Sun, Nov, 2, 60 * -8);   // UTC-8
  ntp.begin();

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_POST, handleData, NULL, handlePostData);

  server.onNotFound(notFound);

  server.begin();
}

void loop() {
  ntp.update();
  delay(1000);
}
