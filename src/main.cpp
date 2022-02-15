//følger denne guiden http://www.iotsharing.com/2017/05/tcp-udp-ip-with-esp32.html
//setter opp en server på ESP32en også ser etter klienter.
//hvis en klient kobler seg på og sender en melding så printer den ut meldingen
//bruker node red i dette tilfellet til å koble opp til ESP32en

#include <Arduino.h>
#include <WiFi.h>
#include "ESPmDNS.h"

#define LED_PIN 2

const char* ssid     = "CompaxNet";
const char* password = "Bosmannen";
/* create a server and listen on port 8088 */
WiFiServer server(8088);
void setup()
{
    Serial.begin(115200);
    Serial.print("Connecting to ");
    Serial.println(ssid);

    pinMode(LED_PIN,OUTPUT);
    delay(100);
    digitalWrite(LED_PIN, HIGH);

    /* connecting to WiFi */
    WiFi.begin(ssid, password);
    /*wait until ESP32 connect to WiFi*/
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected with IP address: ");
    Serial.println(WiFi.localIP());

    if(!MDNS.begin("NODETEST")){
      Serial.println("Error starting mDNS");
    }
    /* start Server */
    server.begin();
}
void loop(){
    /* listen for client */
    WiFiClient client = server.available(); //dette gir muligheten til å ha flere klienter
    uint8_t data[30];
    String Sdata;
    if (client) {                   
      Serial.println("new client");         
      /* check client is connected */           
      while (client.connected()) {          
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