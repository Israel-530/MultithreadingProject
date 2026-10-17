#include "web_interface.h"
#include "config.h"

#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>


//    ----------------------------
const char *ssid     = "XIAO_ESP32S3";
const char *password = "password";
WiFiServer server(80);

void SetupWiFi(){
    Serial.println("\nStarting WiFi");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    WiFi.begin(ssid, password);
    IPAddress apIP = WiFi.softAPIP();
    server.begin();
    //delay(2000);
    //while (WiFi.status() != WL_CONNECTED) {delay(500); Serial.print(".");}
    //Serial.println("Connected. IP: " + WiFi.localIP().toString());
}
//    ----------------------------

void handleWiFi(){
    WiFiClient client = server.available();

    if (!client) return;

    Serial.println("connected");

    while (client.connected()){
        if (!client.available()) continue;

       // char c = client.read();

       // if (c =='l'){

            File file1 = LittleFS.open("/file1.csv", "r");
            if (file1){
                client.print(file1.read());
                client.println("file read");
            } else {
                client.println("No file");
            }
            file1.close();
            

       // }
    }

    client.stop();
}