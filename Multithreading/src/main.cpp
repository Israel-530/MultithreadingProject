/**
  DOCUMENTATION BLOCK
  10/01/26 - !!!!! - Added skeleton code.  Made blinking LEDs.
  10/08/26 - !!!!! - Added necessary code to blink two LEDs on different cores.
  10/13/26 - !!!!! - 
**/

#include <Arduino.h>
#include "config.h"
#include "web_interface.h"
#include <LittleFS.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/*
Project requirements:
  +Write code to blink two LEDs independently on separte cores
  -Record the number of flashes and write to a CSV file
  -View the current light and dowload the CSV file

Additonal considerations:
  Ensure to properly document code
  Refer to CONTRIBUTING.md in the current repository
  Feel free to always ask questions, best of luck!


*/

//
void SetupWiFi();
void handleWiFi();
//

static TaskHandle_t led1Handle = NULL;
static TaskHandle_t led2Handle = NULL;

void LED1Task(void* pvParameters){
  int count1 = 0;
  File file1;

  file1 = LittleFS.open("/file1.csv", "w");

   if (!file1) {
        Serial.println("IMUWriter: FATAL - could not open file.");
    }

  for(;;){      
  count1 += 1;                    
  vTaskDelay(pdMS_TO_TICKS(1000));
  digitalWrite(1,HIGH);
  vTaskDelay(pdMS_TO_TICKS(1000));
  digitalWrite(1, LOW);

  //Serial.println("Print1");
  file1.println(count1);
  file1.flush();

  }
}

void LED2Task(void* pvParameters){
  int count2 = 0;
  File file2;
  file2 = LittleFS.open("/file2.csv", "w");

  if (!file2) {
        Serial.println("IMUWriter: FATAL - could not open file.");
    }/* else {
      Serial.println("Da");
    }*/

  for(;;){
    count2 += 1;
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(2,HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(2, LOW);

    //Serial.println("Print2");
    file2.println(count2);
    file2.flush();
  }

}

void setup() {
  Serial.begin(115200);
  delay(500);
  //LittleFS.remove("/file1.csv");
  //LittleFS.remove("file2.csv");

  pinMode(1,OUTPUT);
  pinMode(2, OUTPUT);

  SetupWiFi();

  xTaskCreatePinnedToCore(LED1Task, "LED1", 2288, NULL, 1, &led1Handle, 0);
  xTaskCreatePinnedToCore(LED2Task, "LED2", 2288, NULL, 1, &led2Handle, 1);

  
}

void loop() {
  handleWiFi();

}
