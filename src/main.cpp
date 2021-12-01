#include <Arduino.h>
#include "AsyncUDP.h"
#include <WiFi.h>

const char * ssid = "CompaxNet";
const char * password = "Bosmannen";

AsyncUDP udp;

uint8_t test_value = 10;
String test_string = "a";
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
}

void loop()
{
    delay(1);
    //Send broadcast on port 1234
    Serial.print(".");
    sprintf(my_str, "%i \n", counter);
    udp.broadcastTo(my_str,2555);

    counter++;
}