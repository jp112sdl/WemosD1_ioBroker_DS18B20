bool loadSystemConfig() {
  printSerial("mounting FS...");
  if (SPIFFS.begin()) {
    printSerial("mounted file system");
    if (SPIFFS.exists("/" + configJsonFile)) {
      printSerial("reading config file");
      File configFile = SPIFFS.open("/" + configJsonFile, "r");
      if (configFile) {
        printSerial("opened config file");
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        if (SerialDEBUG) {
          json.printTo(Serial);
          Serial.println("");
        }
        if (json.success()) {
          printSerial("\nparsed json");
          strcpy(ip,          json["ip"]);
          strcpy(netmask,    json["netmask"]);
          strcpy(gw,         json["gw"]);
          
          String jsonioBrokerIP = json["ioBrokerIP"];
          if (jsonioBrokerIP != "") {
            printSerial("Gehe ins if...");
            strcpy(ioBrokerIP, json["ioBrokerIP"]);
          }
                    
          const char* jsonObjectID = json["ObjectID"];
          ioBrokerObjectID = jsonObjectID;
          
          itoa(json["sleeptime"], sleepTimeMin, 10);
          itoa(json["simpleApiPort"], simpleApiPort, 10);
        } else {
          printSerial("failed to load json config");
        }
      }
      return true;
    } else {
      printSerial("/" + configJsonFile + " not found.");
      return false;
    }
    SPIFFS.end();
  } else {
    printSerial("failed to mount FS");
    return false;
  }
}
