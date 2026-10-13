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

// Declaration of a semaphore handle for synchronizing access to the Serial output
SemaphoreHandle_t serialMutex = NULL;

void LED1Task(void* pvParameters){

  int counter1 = 0;

  for(;;){             
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(1,HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(1, LOW);

    counter1++;
    
    if(xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE){
      Serial.printf("%d,LED1,%lu\n", xPortGetCoreID(),counter1);
      xSemaphoreGive(serialMutex);
    }
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

    // Attempt to take the semaphore before accessing the Serial output
    // If the semaphore is successfully taken, print the LED2 blink count and release the semaphore
    // This ensures that only one task can access the Serial output at a time, preventing data corruption and ensuring thread safety
    /* The xSemaphoreTake function attempts to take the semaphore, and if it is successful, it returns pdTRUE. 
    The portMAX_DELAY parameter specifies that the task should wait indefinitely until the semaphore becomes available. 
    After printing the LED2 blink count, the xSemaphoreGive function is called to release the semaphore, allowing other tasks to access the Serial output.
    
    */
    if(xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
      Serial.printf("%d,LED2,%lu\n",xPortGetCoreID(),counter2);
      xSemaphoreGive(serialMutex);
    }
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

  /**semaphore is a synchronization mechanism, used by tasks to 
  ensure that only one task can access a shared resource at a time. 
  In this case, the shared resource is the Serial output. 
  The semaphore is created using the xSemaphoreCreateMutex() function, 
  which creates a mutex semaphore. If the semaphore creation fails, an 
  error message is printed to the Serial output and the setup function returns early.*/

  /*Mutex is a type of semaphore that allows only one task to access a shared resource at a time.*/

  serialMutex = xSemaphoreCreateMutex();

  // Validation for mutex creation, if it fails, print an error message and return from setup

  if(serialMutex == NULL){
    Serial.println("Failed to create mutex");
    return;
  }

  Serial.println("core,led,blink_count");

  xTaskCreatePinnedToCore(LED1Task, "LED1", 12288, NULL, 1, &led1Handle, 0);
  xTaskCreatePinnedToCore(LED2Task, "LED2", 12288, NULL, 1, &led2Handle, 1);
}

void loop() {
  // put your main code here, to run repeatedly:
}
