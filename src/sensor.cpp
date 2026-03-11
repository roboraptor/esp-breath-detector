#include <Arduino.h>
#include "globals.h"
#include "sensor.h"

// Interní proměnné pouze pro tento soubor
static int readings[WINDOW_SIZE];
static int readIndex = 0;
static long windowTotal = 0;
static int lastValidRaw = 0;
static float lastFilteredValue = 0;
static int localMin = 4095;
static int dynamicMidpoint = 2000;
static const int MAX_STEP = 300;

int getCleanAverage() {
  int raw = analogRead(SENSOR_PIN);

  // WOF - Outlier Rejection
  if (abs(raw - lastValidRaw) > MAX_STEP && lastValidRaw != 0) {
    raw = lastValidRaw; 
  }
  lastValidRaw = raw;

  // Klouzavý průměr
  windowTotal = windowTotal - readings[readIndex];
  readings[readIndex] = raw;
  windowTotal = windowTotal + readings[readIndex];
  readIndex = (readIndex + 1) % WINDOW_SIZE;

  return windowTotal / WINDOW_SIZE;
}

void setupSensor() {
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  analogSetAttenuation(ADC_11db);

  int startVal = analogRead(SENSOR_PIN);
  for (int i = 0; i < WINDOW_SIZE; i++) readings[i] = startVal;
  windowTotal = startVal * WINDOW_SIZE;
}

void loopSensor() {
  float currentAlpha = (currentState == EXHALED) ? ALPHA_HIGH : ALPHA_LOW;
  
  int avgRaw = getCleanAverage();
  currentRaw = avgRaw; // Aktualizace globální proměnné pro WebUI
  
  filteredValue = (currentAlpha * avgRaw) + ((1.0 - currentAlpha) * lastFilteredValue);
  float derivative = filteredValue - lastFilteredValue;

  switch (currentState) {
    case EXHALED:
      if (filteredValue < localMin) localMin = filteredValue;
      if (derivative > 5.0 && filteredValue > (localMin + 40)) {
        currentState = INHALING;
        Serial.println("[EVENT] START NÁDECHU");
      }
      break;

    case INHALING:
      if (derivative < 1.0) {
        currentState = INHALED;
        Serial.println("[STATE] PLNOST");
      }
      break;

    case INHALED:
      if (derivative < -5.0) {
        currentState = EXHALING;
        Serial.println("[STATE] VÝDECH");
      }
      break;

    case EXHALING:
      if (derivative > -1.0) {
        currentState = EXHALED;
        localMin = filteredValue;
        Serial.println("[STATE] PRÁZDNOST - READY");
      }
      break;
  }

  // --- FINÁLNÍ LOGIKA SPÍNÁNÍ ---
  // 1. Force Pump (přebíjí vše)
  // 2. Breathe Pump (jen v povolených stavech)
  // Automatika je nyní vypnutá, spínání je pouze manuální.
  if (cmdForcePump || (cmdBreathePump && (currentState == EXHALED || currentState == INHALING))) {
    digitalWrite(PUMP_PIN, HIGH);
  } else {
    digitalWrite(PUMP_PIN, LOW);
  }

  dynamicMidpoint = (0.01 * filteredValue) + (0.99 * dynamicMidpoint);

  // Serial Plotter formát
  Serial.print("Raw:"); Serial.print(avgRaw);
  Serial.print(",Filtered:"); Serial.print(filteredValue);
  Serial.print(",State:"); Serial.println((int)currentState * 500);

  lastFilteredValue = filteredValue;
}