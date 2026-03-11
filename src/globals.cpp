#include "globals.h"

// Inicializace sdílených proměnných
State currentState = EXHALED;
int currentRaw = 0;
float filteredValue = 0;
bool cmdBreathePump = false;
bool cmdForcePump = false;

String getStateName(State s) {
  switch(s) {
    case INHALING: return "🔼 IN";
    case INHALED:  return "⏸️ HOLD IN";
    case EXHALING: return "🔽 EX";
    case EXHALED:  return "⏸️ HOLD EX";
    default: return "NEZNÁMÝ";
  }
}