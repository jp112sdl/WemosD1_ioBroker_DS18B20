void setStateIoBroker(String type, String value, int Index) {
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    String url = "http://" + String(ioBrokerIP) + ":" + String(simpleApiPort) + "/set/" + String(ioBrokerObjectID) + "." + String(Index) + "?value=" + value + "&wait=100&prettyPrint";
    printSerial("URL = " + url);
    http.begin(url);
    int httpCode = http.GET();
    printSerial("httpcode = " + String(httpCode));
    if (httpCode > 0) {
      //     String payload = http.getString();
    }
    if (httpCode != 200) {
      printSerial("HTTP fail " + String(httpCode));
    }
    http.end();
  } else ESP.restart();
}

