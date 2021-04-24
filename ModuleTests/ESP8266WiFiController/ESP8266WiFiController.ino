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

#include "commons.h"
#include "mainpage.h"
#include "actionpage.h"
#include "calibrationpage.h"

#define BUILTIN_LED 2

ESP8266WebServer server(80);

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

void handleActionPage() {
  server.send(200, "text/html", renderHtml(FPSTR(actionpage), "Actions"));
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
  server.send(200, "text/html", renderHtml(FPSTR(calibrationpage), "Calibration"));
}

void handleAction() {
  String argname = server.arg("name");

  if(argname == "gyro"){              // gyro switch
    Serial.print("g");
  }
  else if(argname == "calibration"){  // calibration mode
    Serial.print("c");
  }
  else if(argname == "balance"){      // balance mode
    Serial.print("kbalance");   
  }
  else if(argname == "walk"){         // demo walk
    Serial.print("kwkF");
  }
  else if(argname == "trot"){         // trot
    Serial.print("ktrF");
  }
  else if(argname == "crawl"){        // crawl
    Serial.print("kcrF");
  }
  else if(argname == "sit"){          // sit
    Serial.print("ksit");
  }
  else if(argname == "check"){        // check 
    Serial.print("kck");
  }
  else if(argname == "buttup"){       // butt UP
    Serial.print("kbuttUp");
  }
  else if(argname == "hello"){        // Say Hi~ 
    Serial.print("khi");
  }
  else if(argname == "stretch"){      // stretch body
    Serial.print("kstr");
  }
  else if(argname == "run"){          // run
    Serial.print("krnF");
  }
  else if(argname == "pee"){          // pee
    Serial.print("kpee");
  }
  else if(argname == "pushup"){       // pushup
    Serial.print("kpu");
  }
  else if(argname == "stepping"){     // stepping
    Serial.print("kvt");
  }
  else if(argname == "lookup"){       // lookup
    Serial.print("lu");
  }
  else if(argname == "forward"){
    Serial.print("kwkF");
  }
  else if(argname == "forwardleft"){
    Serial.print("kwkL");
  }
  else if(argname == "forwardright"){
    Serial.print("kwkR");
  }
  else if(argname == "backleft"){
    Serial.print("kbkL");
  }
  else if(argname == "backright"){
    Serial.print("kbkR");
  }
  else if(argname == "back"){
    Serial.print("kbk");
  }
  else if(argname == "stop"){
    Serial.print("d");
  }
  else {
    Serial.print(argname);      // pass through hhtp argument to Bittle
  }

  // Return to actionpage after CMD
  handleActionPage();
}

void setup(void) {

  // Serial and GPIO LED
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);

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
