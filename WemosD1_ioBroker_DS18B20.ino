#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define TasterPin      D7 //Taster gegen GND, um den Konfigurationsmodus zu aktivieren
#define SensorVCCPin   D1 //+VCC-Pin 
#define DS18B20DataPin D4
#define DebugPin       D5 //Optional: D5 gegen GND, um serielle Ausgabe zu aktivieren (115200,8,N,1)

OneWire DS18B20Wire(DS18B20DataPin);
DallasTemperature DS18B20Sensors(&DS18B20Wire);

char sleepTimeMin[4]  = "60";
char simpleApiPort[8] = "8087";
char ioBrokerIP[16];
String ioBrokerObjectID;
bool SerialDEBUG = false;

//WifiManager - don't touch
byte ConfigPortalTimeout = 180;
bool shouldSaveConfig        = false;
String configJsonFile        = "config.json";
bool wifiManagerDebugOutput = false;
char ip[16]      = "0.0.0.0";
char netmask[16] = "0.0.0.0";
char gw[16]      = "0.0.0.0";
boolean startWifiManager = false;

void setup() {
  pinMode(TasterPin,      INPUT_PULLUP);
  pinMode(DebugPin,       INPUT_PULLUP);
  pinMode(DS18B20DataPin, INPUT_PULLUP);
  pinMode(SensorVCCPin,   OUTPUT);
  pinMode(LED_BUILTIN,    OUTPUT);

  digitalWrite(SensorVCCPin, HIGH);
  digitalWrite(LED_BUILTIN,  HIGH);

  if (digitalRead(DebugPin) == LOW) {
    SerialDEBUG = true;
    wifiManagerDebugOutput = true;
    Serial.begin(115200);
    printSerial("Programmstart...");
  }

  if (digitalRead(TasterPin) == LOW) {
    startWifiManager = true;
    bool state = LOW;
    for (int i = 0; i < 7; i++) {
      state = !state;
      digitalWrite(LED_BUILTIN, state);
      delay(100);
    }
  }

  loadSystemConfig();

  if (doWifiConnect()) {
    printSerial("WLAN erfolgreich verbunden!");
    DS18B20Sensors.begin();
    int sensorCount = DS18B20Sensors.getDeviceCount();
    printSerial("gefundene Sensoren: " + String(sensorCount));
    DS18B20Sensors.requestTemperatures();
    for (int i = 0; i < sensorCount; i++) {
      float t = DS18B20Sensors.getTempCByIndex(i);
      printSerial("Setze ioBroker-Wert Temperatur Sensor(" + String(i+1) + ") = " + String(t));
      setStateIoBroker("SET_TEMPERATURE", String(DS18B20Sensors.getTempCByIndex(i)), i+1);
    }
  } else ESP.restart();
  delay(100);
  printSerial("Gehe schlafen für " + String(sleepTimeMin) + " Minuten");
  ESP.deepSleep((String(sleepTimeMin)).toInt() * 1000000 * 60);
  delay(100);
}

void printSerial(String text) {
  if (SerialDEBUG) {
    Serial.println(text);
  }
}

void loop() {
}
