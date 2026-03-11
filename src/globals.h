#pragma once
#include <Arduino.h>

// --- PINY ---
const int SENSOR_PIN = 34;
const int PUMP_PIN   = 25;

// --- KONFIGURACE ---
const int WINDOW_SIZE = 8;
const float ALPHA_LOW = 0.1;
const float ALPHA_HIGH = 0.4;

// --- STAVOVÁ LOGIKA ---
enum State { INHALING, INHALED, EXHALING, EXHALED };

// Funkce pro převod stavu na text
String getStateName(State s);

// --- SDÍLENÉ PROMĚNNÉ (extern) ---
// Definice jsou v globals.cpp, zde jen deklarujeme dostupnost pro ostatní soubory
extern State currentState;
extern int currentRaw;
extern float filteredValue;

// --- MANUÁLNÍ OVLÁDÁNÍ ---
extern bool cmdBreathePump; // Tlačítko Breathe Pump
extern bool cmdForcePump;   // Tlačítko Force Pump