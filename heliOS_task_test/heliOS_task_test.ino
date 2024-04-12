/*UNCRUSTIFY-OFF*/
/**
 * @file Timer.ino
 * @author Manny Peterson <manny@heliosproj.org>
 * @brief An example Arduino sketch
 * @version 0.4.1
 * @date 2023-03-19
 * 
 * @copyright
 * HeliOS Embedded Operating System Copyright (C) 2020-2023 HeliOS Project <license@heliosproj.org>
 *  
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  
 * 
 */
/*UNCRUSTIFY-ON*/
#include <Arduino.h>
#include <HeliOS.h>


void taskPrint_main(xTask task_, xTaskParm parm_) {
  Serial.println("taskPrint_main(): one second has passed.");
}


void taskUI_main(xTask task_, xTaskParm parm_) {
  Serial.println("taskUI_main(): just looping here.");
}


void setup() {
  xTask task;


  Serial.begin(9600);
  while (!Serial) {}

  Serial.println("heliOS: init");
  
  if(ERROR(xSystemInit())) {
    xSystemHalt();
  }

  Serial.println("heliOS: adding tasks");

//----------------------------------------------------------------------------------
  if(ERROR(xTaskCreate(&task, (const xByte *) "PRINTTSK", taskPrint_main, null))) {
    xSystemHalt();
  }

  if(ERROR(xTaskWait(task))) {
    xSystemHalt();
  }

  if(ERROR(xTaskChangePeriod(task, 1000))) {
    xSystemHalt();
  }
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
  if(ERROR(xTaskCreate(&task, (const xByte *) "UITSK", taskUI_main, null))) {
    xSystemHalt();
  }

  if(ERROR(xTaskWait(task))) {
    xSystemHalt();
  }

  if(ERROR(xTaskChangePeriod(task, 1200))) {
    xSystemHalt();
  }
//----------------------------------------------------------------------------------

  Serial.println("heliOS: start scheduler");
  if(ERROR(xTaskStartScheduler())) {
    xSystemHalt();
  }
  Serial.println("heliOS: scheduler FAILED");

  xSystemHalt();
}


void loop() {

  // will never get here as heliOS keep control
  Serial.println("heliOS is DEAD");

}