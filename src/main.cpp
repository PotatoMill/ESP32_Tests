#include <Arduino.h>
#include "AsyncUDP.h"
#include <WiFi.h>

const char * ssid = "CompaxNet";
const char * password = "Bosmannen";

AsyncUDP udp;

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

    //The server is receiving on port 6309
    //if(udp.listen(6309)) {
    /* if(udp.connect(IPAddress(255,255,255,255), 6309)) {
        Serial.println("UDP connected");
        udp.onPacket([](AsyncUDPPacket packet) {
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
            //reply to the client
            packet.printf("Got %u bytes of data", packet.length());
        });
        //Send unicast
        udp.print("Hello Server!");
    } */
}

void loop()
{
    delay(1000);
    //Send broadcast on port 1234
    Serial.print(".");
    udp.broadcastTo("test", 1234);
}