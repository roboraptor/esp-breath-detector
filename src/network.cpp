#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include "globals.h"
#include "network.h"

WebServer server(80);

String getTasmotaHeader(String title) {
  String h = "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
  h += "<title>" + title + "</title>";
  // Meta refresh odstraněn - o aktualizaci se stará JavaScript
  h += "<style>";
  h += "body{background:#252525;color:#fff;font-family:Verdana,sans-serif;text-align:center;margin:0}";
  h += "div{display:inline-block;width:340px;text-align:left;margin:20px}";
  h += "h2{text-align:center;margin-bottom:20px}";
  h += "table{width:100%;border-spacing:0;background:#353535;border-radius:5px;padding:10px;margin-bottom:10px;box-shadow:0 1px 3px rgba(0,0,0,0.5)}";
  h += "td{padding:8px;border-bottom:1px solid #444}";
  h += "td:last-child{text-align:right;font-weight:bold}";
  h += "button{width:100%;height:46px;margin-bottom:10px;border:0;border-radius:4px;font-size:16px;cursor:pointer;color:#fff;background:#1fa3ec;transition:0.3s}";
  h += "button:hover{background:#0e70a4}";
  h += "button.red{background:#d43535}";
  h += "button.red:hover{background:#931f1f}";
  // Styly pro nová tlačítka
  h += ".btn-row{display:flex;justify-content:space-between;gap:10px;margin:0 0 10px 0}";
  h += ".btn-sq{margin:0;flex:1;height:58px;background:#2ecc71;border:none;border-radius:4px;color:#fff;font-weight:bold;font-size:14px;cursor:pointer;user-select:none;-webkit-user-select:none;touch-action:manipulation;box-shadow:0 2px 0 #27ae60;display:flex;align-items:center;justify-content:center;line-height:1.2}";
  h += ".btn-sq:active{transform:translateY(2px);box-shadow:none;background:#27ae60}";
  h += ".s{font-size:12px;color:#aaa;text-align:center;margin-top:20px}";
  h += "</style></head><body><div>";
  h += "<h2>" + title + "</h2>";
  return h;
}

// Endpoint pro JSON data (voláno AJAXem)
void handleStatus() {
  String json = "{";
  json += "\"state\":\"" + getStateName(currentState) + "\",";
  json += "\"raw\":" + String(currentRaw) + ",";
  json += "\"filt\":" + String(filteredValue, 1) + ",";
  json += "\"pump\":" + String(digitalRead(PUMP_PIN)) + ",";
  json += "\"rssi\":" + String(WiFi.RSSI());
  json += "}";
  server.send(200, "application/json", json);
}

void handleRoot() {
  String page = getTasmotaHeader("Breath Sensor");
  
  // 1. Graf (Nahoře)
  page += "<canvas id='chart' width='340' height='150' style='background:#1a1a1a;border-radius:4px;margin-bottom:10px'></canvas>";

  // 2. Ovládací tlačítka (Uprostřed)
  page += "<div class='btn-row'>";
  page += " <div id='btnBreathe' class='btn-sq'>Breathe<br>Pump</div>";
  page += " <div id='btnForce' class='btn-sq'>Force<br>Pump</div>";
  page += "</div>";

  // 3. Tabulka hodnot (Dole)
  page += "<table>";
  page += "<tr><td>Stav systému</td><td id='val_state'>Načítám...</td></tr>";
  page += "<tr><td>Sensor (Raw)</td><td id='val_raw'>0</td></tr>";
  page += "<tr><td>Sensor (Filter)</td><td id='val_filt'>0</td></tr>";
  page += "<tr><td>Kompresor</td><td id='val_pump'>...</td></tr>";
  page += "<tr><td>WiFi Signál</td><td id='val_rssi'>...</td></tr>";
  page += "</table>";

  page += "<form action='/restart' method='post'><button class='red'>Restartovat zařízení</button></form>";
  
  page += "<p class='s'>ESP32 Breath Controller<br>" + WiFi.localIP().toString() + "</p>";

  // JavaScript pro živý graf a aktualizaci hodnot
  page += "<script>";
  page += "var ctx=document.getElementById('chart').getContext('2d');";
  page += "var data=[];";
  page += "var maxPoints=100;"; // 100 bodů * 50ms = 5 sekund historie
  
  // Funkce pro ovládání tlačítek
  page += "function sendCmd(m){fetch('/cmd/pump?mode='+m);}";
  page += "function setupBtn(id, onCmd, offCmd){";
  page += " var b=document.getElementById(id);";
  page += " b.addEventListener('mousedown', function(e){sendCmd(onCmd);}, false);";
  page += " b.addEventListener('mouseup', function(e){sendCmd(offCmd);}, false);";
  page += " b.addEventListener('mouseleave', function(e){sendCmd(offCmd);}, false);"; // Pojistka při vyjetí myší
  page += " b.addEventListener('touchstart', function(e){e.preventDefault(); sendCmd(onCmd);}, false);";
  page += " b.addEventListener('touchend', function(e){e.preventDefault(); sendCmd(offCmd);}, false);";
  page += "}";
  page += "setupBtn('btnBreathe', 'breathe_on', 'breathe_off');";
  page += "setupBtn('btnForce', 'force_on', 'force_off');";

  // Aktualizační smyčka
  page += "function update(){";
  page += " fetch('/status').then(r=>r.json()).then(j=>{";
  // Aktualizace tabulky
  page += "  document.getElementById('val_state').innerText=j.state;";
  page += "  document.getElementById('val_raw').innerText=j.raw;";
  page += "  document.getElementById('val_filt').innerText=j.filt;";
  page += "  document.getElementById('val_pump').innerText=j.pump?'🟩 ON':'🟥 OFF';";
  page += "  document.getElementById('val_rssi').innerText=j.rssi+' dBm';";
  // Aktualizace dat pro graf
  page += "  data.push(parseFloat(j.filt));";
  page += "  if(data.length>maxPoints) data.shift();";
  // Vykreslení grafu
  page += "  ctx.fillStyle='#1a1a1a'; ctx.fillRect(0,0,340,150);";
  page += "  ctx.strokeStyle='#1fa3ec'; ctx.lineWidth=2; ctx.beginPath();";
  page += "  var min=Math.min(...data); var max=Math.max(...data);";
  page += "  var range=max-min; if(range<50) range=50;"; // Min rozsah aby graf neskákal šumem
  page += "  data.forEach((v,i)=>{";
  page += "   var x=(i/(maxPoints-1))*340;";
  page += "   var y=150-((v-min)/range)*140 - 5;";
  page += "   if(i==0) ctx.moveTo(x,y); else ctx.lineTo(x,y);";
  page += "  });";
  page += "  ctx.stroke();";
  page += " });";
  page += "}";
  page += "setInterval(update, 50);"; // Refresh každých 50ms (20Hz)
  page += "</script>";

  page += "</div></body></html>";

  server.send(200, "text/html", page);
}

void setupNetwork() {
  WiFiManager wifiManager;
  // wifiManager.resetSettings(); // Pro debug

  if (!wifiManager.autoConnect("BreathSensorAP")) {
    Serial.println("Nepodarilo se pripojit, restartuji...");
    delay(3000);
    ESP.restart();
  }

  ArduinoOTA.setHostname("esp32-breath-sensor");
  ArduinoOTA.begin();

  server.on("/", handleRoot);
  server.on("/status", handleStatus); // Registrace nového endpointu
  
  // Endpoint pro ovládání pumpy
  server.on("/cmd/pump", []() {
    String mode = server.arg("mode");
    if (mode == "breathe_on") cmdBreathePump = true;
    else if (mode == "breathe_off") cmdBreathePump = false;
    else if (mode == "force_on") cmdForcePump = true;
    else if (mode == "force_off") cmdForcePump = false;
    server.send(200, "text/plain", "OK");
  });

  server.on("/restart", HTTP_POST, []() {
    server.send(200, "text/plain", "Restarting...");
    delay(500);
    ESP.restart();
  });
  server.begin();
}

void loopNetwork() {
  ArduinoOTA.handle();
  server.handleClient();
}