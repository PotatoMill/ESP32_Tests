#include <Arduino.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define LED_PIN 2

const char* ssid = "CompaxNet";
const char* password = "Bosmannen";

WiFiServer server(8088);

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  pinMode(LED_PIN,OUTPUT);
  delay(100);
  digitalWrite(LED_PIN, HIGH);

  WiFi.mode(WIFI_STA); //unødvendig å definere at den skal fungere som normalt og ikke være scess point
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  //pio run -t upload --upload-port 192.168.1.29

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  server.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
  WiFiClient client = server.available(); //dette gir muligheten til å ha flere klienter
  uint8_t data[30];
  String Sdata;
    if (client) {                            
      /* check client is connected */           
      if (client.connected()) {          
          if (client.available()) {
              int len = client.read(data, 30);
              if(len < 30){
                  data[len] = '\0';  
              }else {
                  data[30] = '\0';
              }    
              Serial.print("client sent: ");            
              Serial.println((char *)data);
          }
          Sdata = (char *)data;
          if(Sdata.indexOf("pinhigh")){
            digitalWrite(LED_PIN, HIGH);
            client.println("switched pin state");
          }
          if(Sdata.indexOf("pinlow")){
            digitalWrite(LED_PIN, LOW);
            client.println("switched pin state");
          }
      }
    }
}