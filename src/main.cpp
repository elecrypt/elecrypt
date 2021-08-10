/*
* HTTPS Secured Client GET Request
* Copyright (c) 2021, Yasin Aktimur
* All rights reserved.
* Connects to WiFi HotSpot. */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

/* Set these to your desired credentials. */
const char *ssid = "yasinaktimur";  //ENTER YOUR WIFI SETTINGS
const char *password = "12345678";
#define LED D5 //Define blinking LED pin


const char *host = "api.bscscan.com";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "8B 36 5F BF DB 6A C8 8A D4 6A B3 0F 27 99 FE 05 AE BB 07 7C";
//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
 delay(1000);
 Serial.begin(115200);
 pinMode(LED, OUTPUT);
 WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
 delay(1000);
 WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot

 WiFi.begin(ssid, password);     //Connect to your WiFi router
 Serial.println("");

 Serial.print("Connecting");
 // Wait for connection
 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
 }

 //If connection successful show IP address in serial monitor
 Serial.println("");
 Serial.print("Connected to ");
 Serial.println(ssid);
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}
//=======================================================================
//                    Blink Led Program
//=======================================================================
void OPENmyBlinker() {
 digitalWrite(LED, HIGH); // Turn the LED off by making the voltage HIGH

}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
 WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

 Serial.println(host);

 Serial.printf("Using fingerprint '%s'\n", fingerprint);
 httpsClient.setFingerprint(fingerprint);
 httpsClient.setTimeout(15000); // 15 Seconds
 delay(1000);

 Serial.print("HTTPS Connecting");
 int r=0; //retry counter
 while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
     delay(100);
     Serial.print(".");
     r++;
 }
 if(r==30) {
   Serial.println("Connection failed");
 }
 else {
   Serial.println("Connected to web");
 }

 String ADCData, getData, Link;
 int adcvalue=analogRead(A0);  //Read Analog value of LDR
 ADCData = String(adcvalue);   //String to interger conversion

 //GET Data
 Link = "/api?module=account&action=balance&address=ADDDRADDDRADDDRADDDRADDDRADDDRADDDRADDDR&tag=latest&apikey=APIKEYAPIKEYAPIKEYAPIKEYAPIKEYAPIKEYAPIKEYAPIKEY";

 Serial.print("requesting URL: ");
 Serial.println(host+Link);

 httpsClient.print(String("GET ") + Link + " HTTP/1.1\r\n" +
              "Host: " + host + "\r\n" +
              "Connection: close\r\n\r\n");

 Serial.println("request sent");

 while (httpsClient.connected()) {
   String line = httpsClient.readStringUntil('\n');
   if (line == "\r") {
     Serial.println("headers received");
     break;
   }
 }

 Serial.println("reply was:");
 Serial.println("==========");
 String line;
 while(httpsClient.available()){
   line = httpsClient.readStringUntil('\n');  //Read Line by Line
   Serial.println(line); //Print response

   // Stream& input;

   StaticJsonDocument<1024> doc;

   DeserializationError error = deserializeJson(doc, line);

   if (error) {
     Serial.print(F("deserializeJson() failed: "));
     Serial.println(error.f_str());
     return;
   }

   const char* status = doc["status"]; // "1"
   const char* message = doc["message"]; // "OK"
   const char* result = doc["result"]; // "102273320000000000"

  long number = atol(result);


   if(number <= 0) {
     Serial.println("you can close led");

     } else {
       Serial.println("you can open led");
     OPENmyBlinker();
       }
   Serial.println("-*-*-*-*result-*-*-*-*-*-");



 }
 Serial.println("==========");
 Serial.println("closing connection");

 delay(10000);  //GET Data at every 10 seconds
}
