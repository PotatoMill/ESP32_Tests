#include <Arduino.h>
#include "AsyncUDP.h"
#include <WiFi.h>
#include "ESPmDNS.h"

const char * ssid = "CompaxNet";
const char * password = "Bosmannen";

AsyncUDP udp;

int counter = 0;
char my_str[10];

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
            delay(1000);
        }
    }

    Serial.println(WiFi.localIP());
    
    if(!MDNS.begin("udpTest")){
        Serial.println("Error starting mDNS");
    }
}

void loop()
{
    delay(10);
    Serial.print(".");
    sprintf(my_str, "/*%i*/", counter);
    udp.broadcastTo(my_str,2555);
    counter++;
}