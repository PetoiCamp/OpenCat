/*  Please add http://arduino.esp8266.com/stable/package_esp8266com_index.json 
 *  to the "Additional boards manager URLs" in Preference 
 *  Then add ESP8266 in "Boards Manager"
 *  Install https://github.com/tzapu/WiFiManager by add .Zip Library
 *  
 *          Petoi WiFi module Config 
 *  Module name: ESP-WROOM-02D / 4MB Flash
 *  Board: Generic ESP8266 Module
 *  Built-in LED: IO2 (Pin 2)
 *  Upload speed 921600 (may downgrade to half automatically)
 *  CPU Frequency: 80MHz (Power saving, lower heat) / 160MHz (High performance)
 *  Crystal: 26MHz
 *  Flash size: 4MB (FS:2MB / OTA:1019KB recommend) 
 *  Flash mode: DQOUT (Compatible)
 *  Flash frequency: 40MHz
 *  Others: Default
 *  
 *  Connecting the WiFi dongle to the serial programmer, search and connect "Bittle-AP" in WiFi
 *  The setup page should pop up directly, or you may visit 192.168.4.1 
 *  Set up the connection to your router 
 *  Check the IP address assigned to the WiFi module, then you can visit the server by your web browser.
 */

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include <Dictionary.h>

#include "commons.h"
#include "mainpage.h"
#include "actionpage.h"
#include "calibrationpage.h"
// for json string constant
#include "actions.h"

#define BUILTIN_LED 2

ESP8266WebServer server(80);

// dict object for action
Dictionary *actions = new Dictionary(30);

String PROGMEM renderHtml(String body, String title) {
  String page;
  page += FPSTR(head);
  page.replace(FPSTR("%TITLE%"), title);
  page += body;
  page += FPSTR(foot);
  return page;
}

void handleMainPage() {
  server.send(200, "text/html", renderHtml(FPSTR(mainpage), "Home"));
}

void handleCalibrationPage() {
  server.send(200, "text/html", renderHtml(FPSTR(calibrationpage), "Calibration"));
  Serial.print("c");
}

void handleCalibration() {
  String joint = server.arg("c");
  String offset = server.arg("o");
    
  if (joint == "s") {
    Serial.print("s");
  } else {
    Serial.print("c" + joint + " " + offset);
  }
  server.send(200, "text/html", "ok");
}

/**
 * @brief get command argument from request and send to opencat
 * @return result
 **/
String sendCmd() {
  String argname = server.arg("name");

  if (actions->search(argname).isEmpty())
  {
      Serial.print(argname);
  }
  else
  {
      Serial.print((*actions)[argname]);
  }

  // read result
  String ret = Serial.readString();
  return ret;
}

/**
 * @brief action page
 **/
void handleActionPage() {
  if (server.hasArg("name"))
      String _ = sendCmd();
  // Return to actionpage after CMD
  server.send(200, "text/html", renderHtml(FPSTR(actionpage), "Actions"));
}

/**
 * @brief handle action api call
 **/
void handleAction()
{
  String ret = sendCmd();
  server.send(200, "text/plain", ret);
}

void setup(void) {

  // Serial and GPIO LED
  Serial.begin(115200);

  // this number should match to wait time in sender
  Serial.setTimeout(40);

  pinMode(BUILTIN_LED, OUTPUT);

  // load json
  actions->jload(dict_json);

  // WiFiManager
  WiFiManager wifiManager;

  // Start WiFi manager, default gateway IP is 192.168.4.1
  wifiManager.autoConnect("Bittle-AP");
  digitalWrite(BUILTIN_LED, HIGH);      // While connected, LED lights

  // Print the IP get from DHCP Server of your Router
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // HTTP server started with handlers
  server.on("/", handleMainPage);
  server.on("/actionpage", handleActionPage);
  server.on("/action", handleAction);
  server.on("/calibrationpage", handleCalibrationPage);
  server.on("/calibration", handleCalibration);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  // handle clients
  server.handleClient();
}
