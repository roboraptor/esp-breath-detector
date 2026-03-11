# ESP32 Breath Controller (Elastomer Stretch Sensor)

Tento projekt implementuje systém pro detekci dechu pomocí **vodivé elastomerové gumové trubičky (stretch sensor)**. Systém na základě analýzy dechové křivky ovládá vzduchový kompresor (např. pro aroma terapii, asistované dýchání nebo biofeedback).

Projekt obsahuje pokročilé webové rozhraní ve stylu **Tasmota** s živým grafem, možností manuálního ovládání a OTA aktualizací.

## 🧬 Princip fungování senzoru

Jako senzor se využívá speciální **uhlíková vodivá guma** (elastomer cord).

1.  Guma se upevní kolem hrudníku (např. pomocí kineziologické pásky nebo popruhu).
2.  Při nádechu se hrudník roztáhne -> guma se natáhne.
3.  **Změna odporu:** Při natažení se mění elektrický odpor materiálu.
4.  ESP32 měří tento odpor přes dělič napětí na analogovém vstupu a softwarově detekuje fáze dechu (nádech/výdech).

## ⚙️ Hardware

*   **MCU:** ESP32 (DevKit V1 nebo kompatibilní)
*   **Senzor:** Vodivá guma (Conductive Rubber Cord / Stretch Sensor)
*   **Akční člen:** Relé například pro Mini vzduchový kompresor (5V/12V)
*   **Spínání:** MOSFET tranzistor (např. IRLZ44N) nebo relé modul
*   **Rezistor:** 10kΩ - 47kΩ (pro dělič napětí se senzorem)

### Zapojení (Pinout)

| Komponenta | ESP32 Pin | Poznámka |
| :--- | :--- | :--- |
| **Senzor (ADC)** | `GPIO 34` | Analogový vstup (zapojeno jako dělič napětí) |
| **Kompresor (PWM)** | `GPIO 25` | Výstup pro Gate MOSFETu |

*Poznámka: Senzor zapojte do série s rezistorem mezi 3.3V a GND. Bod mezi senzorem a rezistorem připojte na GPIO 34.*

## 🖥️ Webové rozhraní (WebUI)

Po připojení k WiFi (nebo přes AP) je dostupné rozhraní na IP adrese ESP32 (port 80).

*   **Design:** Tasmota Dark Theme (vhodné pro mobilní telefony).
*   **Live Chart:** Živý graf dechové křivky (canvas, 20Hz refresh).
*   **Indikátory:**
    *   `🔼 IN` - Nádech (Inhaling)
    *   `⏸️ HOLD IN` - Zadržení dechu (Plnost)
    *   `🔽 EX` - Výdech (Exhaling)
    *   `⏸️ HOLD EX` - Klidový stav (Vydechnuto)
*   **Ovládání:**
    *   **Breathe Pump:** Spíná kompresor pouze pokud je detekován nádech nebo klidový stav (podržení tlačítka).
    *   **Force Pump:** Spíná kompresor okamžitě bez ohledu na dech (manuální override).

## 🚀 Instalace a Flashování

Projekt je postaven na frameworku **PlatformIO**.

1.  Otevřete složku projektu v VS Code s rozšířením PlatformIO.
2.  Připojte ESP32 přes USB.
3.  Nahrajte firmware (tlačítko šipky v dolní liště nebo `PlatformIO: Upload`).

### První spuštění (WiFi Manager)

1.  Po nahrání kódu, pokud ESP32 nezná vaši WiFi, vytvoří přístupový bod (Hotspot) s názvem:
    *   SSID: **BreathSensorAP**
    *   Heslo: *(bez hesla)*
2.  Připojte se telefonem k této síti.
3.  Automaticky vyskočí konfigurační stránka (Captive Portal).
4.  Vyberte svou domácí WiFi, zadejte heslo a uložte.
5.  ESP32 se restartuje a připojí do vaší sítě.

## 🧩 Struktura kódu

*   `src/main.cpp` - Hlavní smyčka a inicializace.
*   `src/sensor.cpp` - Logika čtení ADC, filtrace šumu a detekce dechu.
*   `src/network.cpp` - WiFiManager, WebServer, HTML/JS generátor.
*   `src/globals.cpp` - Sdílené proměnné a konfigurace.

## 🛠️ Ladění

Pro ladění senzoru otevřete **Serial Monitor** (baudrate 115200) nebo **Serial Plotter**. Uvidíte surová data, filtrovaná data a detekované stavy v reálném čase.
