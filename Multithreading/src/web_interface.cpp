#include "web_interface.h"
#include "config.h"

#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <Arduino.h>

// These variables are defined in main.cpp
extern volatile bool led1State;
extern volatile bool led2State;

// Start a web server on port 80
WebServer server(80);

void beginWebInterface() {
  WiFi.mode(WIFI_AP);

  if (!WiFi.softAP(WIFI_AP_NAME, WIFI_AP_PASSWORD)) {
    Serial.println("Failed to start Wi-Fi");
    return;
  }

  Serial.print("Connect to: ");
  Serial.println(WIFI_AP_NAME);

  Serial.print("Open: http://");
  Serial.println(WiFi.softAPIP());

  // Dashboard webpage
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <title>ESP32 LED Dashboard</title>
      </head>
      <body>
        <h1>ESP32 LED Dashboard</h1>

        <p>LED 1: <strong id="led1">Loading...</strong></p>
        <p>LED 2: <strong id="led2">Loading...</strong></p>

        <p><a href="/download">Download CSV file</a></p>

        <script>
          async function updateStatus() {
            try {
              const response = await fetch('/status');
              const data = await response.json();

              document.getElementById('led1').textContent =
                data.led1 ? 'ON' : 'OFF';

              document.getElementById('led2').textContent =
                data.led2 ? 'ON' : 'OFF';
            } catch (error) {
              console.log('Could not update LED status');
            }
          }

          updateStatus();
          setInterval(updateStatus, 1000);
        </script>
      </body>
      </html>
    )rawliteral");
  });

  // Current LED states
  server.on("/status", HTTP_GET, []() {
    String response = "{\"led1\":";
    response += (led1State ? "true" : "false");
    response += ",\"led2\":";
    response += (led2State ? "true" : "false");
    response += "}";

    server.send(200, "application/json", response);
  });

  // Download the saved CSV
  server.on("/download", HTTP_GET, []() {
    File downloadFile = LittleFS.open("led_blink_data.csv", "r");

    if (!downloadFile) {
      server.send(404, "text/plain", "CSV file not found");
      return;
    }

    server.sendHeader(
      "Content-Disposition",
      "attachment; filename=led_blink_count.csv"
    );

    server.streamFile(downloadFile, "text/csv");
    downloadFile.close();
  });

  server.begin();
  Serial.println("Web server started");
}

void handleWebInterface() {
  server.handleClient();
}

