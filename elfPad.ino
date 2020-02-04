
#include <ESPWiFi.h>
#include <ESPHTTPClient.h>
#include <JsonListener.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>



// time
#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval
#include <coredecls.h>                  // settimeofday_cb()

#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"
#include "Wire.h"
#include "Fonts.h"
#include "Images.h"


ESP8266WebServer server(80);
String stringtopic = "~~ North Pole HQ Chatroom ~~";
String stringsanta = "*<|:-)";
String stringelfy = "Yo yo yo! Elfy here! *<|:-)";

String stringtopic2 = "~~ North Pole HQ Chatroom ~~";
String stringsanta2 = "HO HO HO!";
String stringelfy2 = "Hey boss!";

String stringtopic3 = "";
String stringsanta3 = "Ho ho?!";
String stringelfy3 = "all clear! chimney prepped!";

String stringtopic4 = "";
String stringsanta4 = "Ho hO, HO!";
String stringelfy4 = "XMAS is on schedule!";

String stringtopic5 = "Ho ho ho, ready to go?";
String stringsanta5 = "Ho, ho ha!";
String stringelfy5 = "";

String stringtopic6 = "";
String stringsanta6 = "Cookies?";
String stringelfy6 = "Yes, macaroons too!";



/***************************
 * Begin Settings
 **************************/


// WIFI
const char* WIFI_SSID = "yourSSIDname";
const char* WIFI_PWD = "yourSSIDpassword";

#define TZ              -8       // (utc+) TZ in hours
#define DST_MN          60      // use 60mn for summer time in some countries

// Setup

// Display Settings
const int I2C_DISPLAY_ADDRESS = 0x3c;
#if defined(ESP8266)
const int SDA_PIN = D3;
const int SDC_PIN = D4;
#else
const int SDA_PIN = 5; //D3;
const int SDC_PIN = 4; //D4;
#endif

/***************************
 * End Settings
 **************************/




 // Initialize the oled display for address 0x3c
 // sda-pin=14 and sdc-pin=12
 SSD1306Wire     display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);
 OLEDDisplayUi   ui( &display );


#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)
time_t now;

// declaring prototypes
void drawProgress(OLEDDisplay *display, int percentage, String label);
void updateData(OLEDDisplay *display);
void drawSnowFlake1UI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawSnowFlake2UI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawSnowFlake3UI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawSnowFlake4UI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawSnowFlake5UI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawSnowFlake6UI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);

void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);


// Add frames
// this array keeps function pointers to all frames
// frames are the single views that slide from right to left
FrameCallback frames[] = { drawSnowFlake1UI, drawSnowFlake2UI, drawSnowFlake3UI, drawSnowFlake4UI, drawSnowFlake5UI, drawSnowFlake6UI };
int numberOfFrames = 6;

OverlayCallback overlays[] = { drawHeaderOverlay };
int numberOfOverlays = 1;


void setup() {
  //Serial.begin(115200);

  //this pin d2 will be used for the local reset BUT don't leave flyer connected during programming!!!
  pinMode(D2, OUTPUT);
  digitalWrite(D2, HIGH);  

  // initialize dispaly
  display.init();
  display.clear();
  display.display();

  //display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setContrast(255);

  WiFi.begin(WIFI_SSID, WIFI_PWD);

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.clear();
    display.drawString(64, 10, "Connecting to North Pole");
    display.drawXbm(46, 30, 8, 8, counter % 3 == 0 ? activeSymbole : inactiveSymbole);
    display.drawXbm(60, 30, 8, 8, counter % 3 == 1 ? activeSymbole : inactiveSymbole);
    display.drawXbm(74, 30, 8, 8, counter % 3 == 2 ? activeSymbole : inactiveSymbole);
    display.display();

    counter++;
  }

    delay(1500);
    display.clear();
    display.drawString(64, 10, "Connected to North Pole");
    String lcladdr = WiFi.localIP().toString().c_str();
    //print elfos ipaddr for adminz
    display.drawString(64, 30, lcladdr);
    display.display();
    delay(1500);

  // Get time from network time service
  //configTime(TZ_SEC, DST_SEC, "pool.ntp.org");

  server.on("/", handleRoot);
  //server.on("/", handleElf);
  server.onNotFound(handleNotFound);

  server.begin();


  ui.setTargetFPS(30);

  ui.setActiveSymbol(activeSymbole);
  ui.setInactiveSymbol(inactiveSymbole);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_TOP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  ui.setFrames(frames, numberOfFrames);

  ui.setOverlays(overlays, numberOfOverlays);

  // Inital UI takes care of initalising the display too.
  ui.init();

  updateData(&display);

    display.clear();
    display.drawString(64, 24, "*<|:-)");
    display.display();
    delay(600);

  delay(5);

}
//endfunc

////start webserver code:

const char INDEX_HTML[] =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
"<title>ESP8266 Elfy Web Server</title>"
"<style>"
"\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\""
"</style>"
"</head>"
"<body>"
"<center><h1>elfOS Web Form</h1><center>"
"<FORM action=\"/\" method=\"post\">"
"<P>"
"<CENTER>"
"Inputs<br>"
"<INPUT type=\"text\" name=\"topic\"<BR>"
"<br>"
"<INPUT type=\"text\" name=\"santa\"<BR>"
"<br>"
"<INPUT type=\"text\" name=\"elfy\"<BR>"
"<BR>"
"<INPUT type=\"submit\" value=\"Send\">"
"</CENTER>"
"</FORM>"
"</body>"
"</html>";

void handleRoot()
{
  if (server.hasArg("topic")) {
    handleSubmit();
  }
  else if (server.hasArg("santa")) {
    handleSubmit();
  }
  else if (server.hasArg("elfy")) {
    handleSubmit();
  }
  else {
    server.send(200, "text/html", INDEX_HTML);
  }
}

//probably will remain unused
void returnFail(String msg)
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
}

void handleSubmit()
{
  
  //if (!server.hasArg("santa")) return returnFail("BAD ARGS");
  if (server.hasArg("topic")) {
    stringtopic = server.arg("topic");
  stringtopic2 = stringtopic;
  stringtopic3 = stringtopic;
  stringtopic4 = stringtopic;
  stringtopic5 = stringtopic;
  stringtopic6 = stringtopic;
  }
  else {
    stringtopic = "~~ North Pole HQ Chatroom ~~";
  }

  if (server.hasArg("santa")) {
    stringsanta = server.arg("santa");
  stringsanta2 = stringsanta;
  stringsanta3 = stringsanta;
  stringsanta4 = stringsanta;
  stringsanta5 = stringsanta;
  stringsanta6 = stringsanta;
  }
  else {
    stringsanta = "Ho Ho Ho!";
  }

  if (server.hasArg("elfy")) {
    stringelfy = server.arg("elfy");
  stringelfy2 = stringelfy;
  stringelfy3 = stringelfy;
  stringelfy4 = stringelfy;
  stringelfy5 = stringelfy;
  stringelfy6 = stringelfy;
  }
  else {
    stringelfy = "XMas will be great!";
  }

  if (stringtopic == "reset") {
    //ESP.reset(); //this crap doesn't work
    digitalWrite(D2, LOW); 
  }
  if (stringsanta == "reset") {
    //ESP.reset(); //this crap doesn't work
    digitalWrite(D2, LOW); 
  }
  if (stringelfy == "reset") {
    //ESP.restart(); //this crap doesn't work
    digitalWrite(D2, LOW);  
  }


  server.send(200, "text/html", INDEX_HTML);
}

void returnOK()
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK\r\n");
}


void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

////end webserver code:

void loop() {

  ////move to budget?
  server.handleClient();

  int remainingTimeBudget = ui.update();


  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    // ...
    // no more work, ur honor
    delay(remainingTimeBudget);
  }


}
//endfunc

void drawProgress(OLEDDisplay *display, int percentage, String label) {
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64, 10, label);
  display->drawProgressBar(2, 28, 124, 10, percentage);
  display->display();
}
//endfunc

void updateData(OLEDDisplay *display) {
  drawProgress(display, 10, "Updating elfPad...");
  delay(1250);
  drawProgress(display, 50, "Updating CandyCaneApp...");
  delay(1250);
  drawProgress(display, 100, "Starting NP chat...");
  delay(1250);

}
//endfunc

void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  now = time(nullptr);

  display->setColor(WHITE);
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0, 52, "elfOS"); //was buff casted as string
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  String temp = "*<|:-)";
  display->drawString(128, 52, temp);
  display->drawHorizontalLine(0, 50, 128);
}
//endfunc



//@@drawSnowFlake1UI
void drawSnowFlake1UI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 0, stringtopic);

  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(63 + x, 12  + y - 3, stringsanta);

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 38  + y - 3, stringelfy);

}
//endfunc




//@@drawSnowFlake2UI
void drawSnowFlake2UI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 0, stringtopic2);

  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(63 + x, 12  + y - 3, stringsanta2);

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 38  + y - 3, stringelfy2);

}
//endfunc



//@@drawSnowFlake3UI
void drawSnowFlake3UI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 0, stringtopic3);

  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(63 + x, 12  + y - 3, stringsanta3);

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 38  + y - 3, stringelfy3);

}
//endfunc


//@@drawSnowFlake4UI
void drawSnowFlake4UI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 0, stringtopic4);

  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(63 + x, 12  + y - 3, stringsanta4);

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 38  + y - 3, stringelfy4);

}
//endfunc


//@@drawSnowFlake5UI
void drawSnowFlake5UI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 0, stringtopic5);

  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(63 + x, 12  + y - 3, stringsanta5);

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 38  + y - 3, stringelfy5);

}
//endfunc



//@@drawSnowFlake6UI
void drawSnowFlake6UI(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 0, stringtopic6);

  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(63 + x, 12  + y - 3, stringsanta6);

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 38  + y - 3, stringelfy6);

}
//endfunc



