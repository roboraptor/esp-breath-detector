/*
 * BREATHE-TRIGGERED AROMA SYSTEM (ESP32)
 * Sensor: Conductive Rubber Cord (Stretch Sensor) on Kinesio-tape
 * Output: Mini Compressor via MOSFET (Pin 25)
 */

#include <Arduino.h>
#include "globals.h"
#include "network.h"
#include "sensor.h"

void setup() {
  Serial.begin(115200);
  setupSensor();
  setupNetwork();
  
  Serial.println("\nWiFi pripojeno. IP: " + WiFi.localIP().toString());
  Serial.println(">>> SYSTÉM AKTIVNÍ - ČEKÁM NA DECH <<<");
}

void loop() {
  // 1. Síťová vrstva (rychlá, bez blokování)
  loopNetwork();

  // 2. Senzorová vrstva (časovaná, cca 66Hz)
  static unsigned long lastLoopTime = 0;
  if (millis() - lastLoopTime < 15) {
    return;
  }
  lastLoopTime = millis();

  loopSensor();
}