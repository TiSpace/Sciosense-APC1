/* **************************************************
*
*        ScioSense APC1 goes Prometheus
*         based on https://github.com/highonvoltageen/Sciosense-APC1-ESP32/tree/main
*
*       requires APC1 with UART interface
*
*  **************************************************
*/

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
//#include <SoftwareSerial.h>
#include <apc1.h>

using namespace ScioSense;

#define rxPin 16
#define txPin 17

APC1 apc1;
 
const char* ssid = "Andromeda";
const char* password =  "Plejaden";
 
IPAddress local_IP(192, 168, 178, 141);
// Set your Gateway IP address
IPAddress gateway(192, 168, 178, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 178, 1);   //optional
IPAddress secondaryDNS(192, 168, 178, 0); //optional

AsyncWebServer server(80);

String PM1_0 = "";
String PM2_5 = "";
String PM10 = "";

String PM1_0IA = "";
String PM2_5IA = "";
String PM10IA = "";

String Pb0_3u = "";
String Pb0_5u = "";
String Pb1u = "";
String Pb2_5u = "";
String Pb5u = "";
String Pb10u = "";

String TVOC = "";
String ECO2 = "";

String TCOMP = "";
String RHCOMP = "";

String TRR = "";
String RHR = "";

String RS0 = "";
String RS1 = "";
String RS2 = "";
String RS3 = "";

String AQI = "";
 
void setup(){
  Serial.begin(115200);
  Serial.println("\n\n APC1 to Prometheus");
  Serial.println(__FILE__);
  Serial.print(__DATE__);Serial.print("  ");
  Serial.println(__TIME__);
  
  pinMode(15,OUTPUT);
  digitalWrite(15,HIGH);
 
  Serial1.begin(9600, SERIAL_8N1, rxPin, txPin);
  apc1.begin(Serial1);

  if (apc1.isConnected() == false)
  {
       Serial.println("Error -- The APC1 is not connected.");
      while(1);
  }

  Serial.print("APC1 FW version: ");
  Serial.println(apc1.getFirmwareVersion());


  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println(WiFi.localIP());
 
  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "APC1 goes Prometheus");
  });
 
  server.on("/metrics", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("text/plain");
    response->print("TEST 0\n");
    response->print("PM2_5 " + PM2_5 + "\n");
    response->print("PM1_0 " + PM1_0 + "\n");
    response->print("PM10 " + PM10 + "\n");
    response->print("PM1_0IA " + PM1_0IA + "\n");
    response->print("PM2_5IA " + PM2_5IA + "\n");
    response->print("PM10IA " + PM10IA + "\n");
    response->print("Pb0_3u " + Pb0_3u + "\n");
    response->print("Pb0_5u " + Pb0_5u + "\n");
    response->print("Pb1u " + Pb1u + "\n");
    response->print("Pb2_5u " + Pb2_5u + "\n");
    response->print("Pb5u " + Pb5u + "\n");
    response->print("Pb10u " + Pb10u + "\n");
    response->print("TVOC " + TVOC + "\n");
    response->print("ECO2 " + ECO2 + "\n");
    response->print("TCOMP " + TCOMP + "\n");
    response->print("RHCOMP " + RHCOMP + "\n");
    response->print("TRR " + TRR + "\n");
    response->print("RHR " + RHR + "\n");
    response->print("RS0 " + RS0 + "\n");
    response->print("RS1 " + RS1 + "\n");
    response->print("RS2 " + RS2 + "\n");
    response->print("RS3 " + RS3 + "\n");
    response->print("AQI " + AQI);
    response->addHeader("charset", "UTF-8");
    response->addHeader("version", "0.0.4");
    response->addHeader("Connection", "close");
    request->send(response);
  });
  server.begin();
  Serial.println("\t\t\t\t\\t\t-- Metric fetched --");
  digitalWrite(15,LOW);
}
 
void loop()
{
 
 
  if (apc1.update() == APC1::Result::Ok)
  {

      PM1_0=apc1.getPM_1_0();
      PM2_5=apc1.getPM_2_5();
      PM10=apc1.getPM_10();

      PM1_0IA=apc1.getPMInAir_1_0();
      PM2_5IA=apc1.getPMInAir_2_5();
      PM10IA=apc1.getPMInAir_10();

      Pb0_3u=apc1.getNoParticles_0_3();
      Pb0_5u=apc1.getNoParticles_0_5();
      Pb1u=apc1.getNoParticles_1_0();
      Pb2_5u=apc1.getNoParticles_2_5();
      Pb5u=apc1.getNoParticles_5_0();
      Pb10u=apc1.getNoParticles_10();

      TVOC=apc1.getTVOC();
      ECO2=apc1.getECO2();

      TCOMP=apc1.getCompT();
      RHCOMP=apc1.getCompRH();

      TRR=apc1.getCompT();
      RHR=apc1.getCompRH();

      RS0=apc1.getRS0();
      RS1=apc1.getRS1();
      RS2=apc1.getRS2();
      RS3=apc1.getRS3();

      AQI=String((uint8_t)apc1.getAQI());

      Serial.print("PM1.0: ");
      Serial.println(PM1_0);

      Serial.print("PM2.5: ");
      Serial.println(PM2_5);

      Serial.print("PM10: ");
      Serial.println(PM10);

      Serial.print("PM1.0 in air: ");
      Serial.println(PM1_0IA);

      Serial.print("PM2.5 in air: ");
      Serial.println(PM2_5IA);

      Serial.print("PM10 in air: ");
      Serial.println(PM10IA);

      Serial.print("# particles >0.3μm: ");
      Serial.println(Pb0_3u);

      Serial.print("# particles >0.5μm: ");
      Serial.println(Pb0_5u);

      Serial.print("# particles >1.0μm: ");
      Serial.println(Pb1u);

      Serial.print("# particles >2.5μm: ");
      Serial.println(Pb2_5u);

      Serial.print("# particles >5.0μm: ");
      Serial.println(Pb5u);

      Serial.print("# particles >10μm: ");
      Serial.println(Pb10u);

      Serial.print("TVOC: ");
      Serial.println(TVOC);

      Serial.print("ECO2: ");
      Serial.println(ECO2);

      Serial.print("T-comp.: ");
      Serial.println(TCOMP);

      Serial.print("RH-comp.: ");
      Serial.println(RHCOMP);

      Serial.print("T-raw: ");
      Serial.println(TRR);

      Serial.print("RH-raw: ");
      Serial.println(RHR);

      Serial.print("RS0: ");
      Serial.println(RS0);

      Serial.print("RS1: ");
      Serial.println(RS1);

      Serial.print("RS2: ");
      Serial.println(RS2);

      Serial.print("RS3: ");
      Serial.println(RS3);

      Serial.print("AQI: ");
      Serial.println(AQI);

      Serial.print("Error code: ");
      Serial.println((uint8_t)apc1.getError(), BIN);

      Serial.println("-----------------------");
  }
  
delay(2);
while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
}