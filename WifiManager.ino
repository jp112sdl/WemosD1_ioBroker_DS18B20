bool doWifiConnect() {
  String _ssid = WiFi.SSID();
  String _psk = WiFi.psk();

  const char* ipStr = ip; byte ipBytes[4]; parseBytes(ipStr, '.', ipBytes, 4, 10);
  const char* netmaskStr = netmask; byte netmaskBytes[4]; parseBytes(netmaskStr, '.', netmaskBytes, 4, 10);
  const char* gwStr = gw; byte gwBytes[4]; parseBytes(gwStr, '.', gwBytes, 4, 10);

  WiFiManager wifiManager;
  wifiManager.setDebugOutput(wifiManagerDebugOutput);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  WiFiManagerParameter custom_ioBrokerIP("ioBroker", "ioBroker IP", ioBrokerIP, 16);
  WiFiManagerParameter custom_simpleApiPort("Port", "simpleApi Port", simpleApiPort, 168);

  char ioBrokerObjectIDasChar[128];
  ioBrokerObjectID.toCharArray(ioBrokerObjectIDasChar, 128);
  WiFiManagerParameter custom_iobrokerObjectID("ObjectID", "ioBroker Objekt ID", ioBrokerObjectIDasChar, 128);
  WiFiManagerParameter custom_sleeptime("sleeptime", "&Uuml;bertragung alle x Minuten", sleepTimeMin, 4);

  WiFiManagerParameter custom_ip("custom_ip", "IP-Adresse", "", 16);
  WiFiManagerParameter custom_netmask("custom_netmask", "Netzmaske", "", 16);
  WiFiManagerParameter custom_gw("custom_gw", "Gateway", "", 16);

  wifiManager.addParameter(&custom_ioBrokerIP);
  wifiManager.addParameter(&custom_simpleApiPort);
  wifiManager.addParameter(&custom_iobrokerObjectID);
  wifiManager.addParameter(&custom_sleeptime);
  WiFiManagerParameter custom_text("<br/><br>Statische IP (wenn leer, dann DHCP):");
  wifiManager.addParameter(&custom_text);
  wifiManager.addParameter(&custom_ip);
  wifiManager.addParameter(&custom_netmask);
  wifiManager.addParameter(&custom_gw);

  String Hostname = "WemosD1-" + WiFi.macAddress();
  char a[] = "";
  Hostname.toCharArray(a, 30);

  wifiManager.setConfigPortalTimeout(ConfigPortalTimeout);

  if (startWifiManager == true) {
    digitalWrite(LED_BUILTIN, LOW);
    if (_ssid == "" || _psk == "" ) {
      wifiManager.resetSettings();
    }
    else {
      if (!wifiManager.startConfigPortal(a)) {
        printSerial("failed to connect and hit timeout");
        delay(1000);
        ESP.restart();
      }
    }
  }

  wifiManager.setSTAStaticIPConfig(IPAddress(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]), IPAddress(gwBytes[0], gwBytes[1], gwBytes[2], gwBytes[3]), IPAddress(netmaskBytes[0], netmaskBytes[1], netmaskBytes[2], netmaskBytes[3]));

  wifiManager.autoConnect(a);

  printSerial("Wifi Connected");
  printSerial("CUSTOM STATIC IP: " + String(ip) + " Netmask: " + String(netmask) + " GW: " + String(gw));
  if (shouldSaveConfig) {
    SPIFFS.begin();
    printSerial("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    if (String(custom_ip.getValue()).length() > 5) {
      printSerial("Custom IP Address is set!");
      strcpy(ip, custom_ip.getValue());
      strcpy(netmask, custom_netmask.getValue());
      strcpy(gw, custom_gw.getValue());
    } else {
      strcpy(ip,      "0.0.0.0");
      strcpy(netmask, "0.0.0.0");
      strcpy(gw,      "0.0.0.0");
    }
    strcpy(ioBrokerIP, custom_ioBrokerIP.getValue());
    ioBrokerObjectID = custom_iobrokerObjectID.getValue();
    json["ip"] = ip;
    json["netmask"] = netmask;
    json["gw"] = gw;
    json["ioBrokerIP"] = ioBrokerIP;
    json["simpleApiPort"] = simpleApiPort;
    json["ObjectID"] = ioBrokerObjectID;
    json["sleeptime"] = String(custom_sleeptime.getValue()).toInt();
    json["simpleApiPort"] = String(custom_simpleApiPort.getValue()).toInt();

    SPIFFS.remove("/" + configJsonFile);
    File configFile = SPIFFS.open("/" + configJsonFile, "w");
    if (!configFile) {
      printSerial("failed to open config file for writing");
    }

    if (SerialDEBUG) {
      json.printTo(Serial);
      Serial.println("");
    }
    json.printTo(configFile);
    configFile.close();

    SPIFFS.end();
    delay(100);
    ESP.restart();
  }
  return true;
}


void configModeCallback (WiFiManager *myWiFiManager) {
  printSerial("AP-Modus ist aktiv!");
}

void saveConfigCallback () {
  printSerial("Should save config");
  shouldSaveConfig = true;
}

void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
  for (int i = 0; i < maxBytes; i++) {
    bytes[i] = strtoul(str, NULL, base);
    str = strchr(str, sep);
    if (str == NULL || *str == '\0') {
      break;
    }
    str++;
  }
}
