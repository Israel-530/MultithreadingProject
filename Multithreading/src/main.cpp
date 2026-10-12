#include <Arduino.h>
#include "config.h"
#include "web_interface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/*
Project requirements:
  -Write code to blink two LEDs independently on separte cores
  -Record the number of flashes and write to a CSV file
  -View the current light and dowload the CSV file

Additonal considerations:
  Ensure to properly document code
  Refer to CONTRIBUTING.md in the current repository
  Feel free to always ask questions, best of luck!


*/
static TaskHandle_t led1Handle = NULL;
static TaskHandle_t led2Handle = NULL;

void LED1Task(void* pvParameters){

  int counter1 = 0;

  for(;;){             

  vTaskDelay(pdMS_TO_TICKS(1000));
  digitalWrite(1,HIGH);
  vTaskDelay(pdMS_TO_TICKS(1000));
  digitalWrite(1, LOW);

  counter1++;
  Serial.print("LED 1 blinks: ");
  Serial.println(counter1);
  }
}

void LED2Task(void* pvParameters){

  int counter2 = 0;

  for(;;){
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(2,HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(2, LOW);

    counter2++;
    Serial.print("LED 2 blinks: ");
    Serial.println(counter2);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);

  pinMode(1,OUTPUT);
  pinMode(2, OUTPUT);

  xTaskCreatePinnedToCore(LED1Task, "LED1", 2288, NULL, 1, &led1Handle, 0);
  xTaskCreatePinnedToCore(LED2Task, "LED2", 2288, NULL, 1, &led2Handle, 1);

  
}

void loop() {
  // put your main code here, to run repeatedly:


}
