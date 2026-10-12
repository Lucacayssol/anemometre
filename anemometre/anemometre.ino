#include <DIYables_ESP32_WebServer.h>

TaskHandle_t WebTaskHandle = NULL;
TaskHandle_t IntervalleTaskHandle = NULL;

// WiFi credentials
const char WIFI_SSID[] = "SSID_Wifi";
const char WIFI_PASSWORD[] = "MdP_Wifi";
float last_passage,time_actuel,t;
int sensorValue;
float multiplicateur = 10;
float vitesse = 0;

// Create web server instance
DIYables_ESP32_WebServer server;

// Page handlers
void handleHome(WiFiClient& client, const String& method, const String& request, const QueryParams& params, const String& jsonData) {
  // get vitesse from sensor
  int temperature = vitesse;
  // Format the vitesse with two decimal places
  String vitesseStr = String(vitesse, 2);

  String html = "<!DOCTYPE HTML>";
  html += "<html>";
  html += "<head>";
  html += "<link rel=\"icon\" href=\"data:,\">";
  html += "</head>";
  html += "<p>";
  html += "Passage: <span style=\"color: red;\">";
  html += vitesse;
  html += "</span>";
  html += "</p>";
  html += "</html>";

  server.sendResponse(client, html.c_str());
}

void WebTask(void *parameter) {
  while (1) {
    server.handleClient();
    vTaskDelay(20);
  }
}

void IntervalleTask(void *parameter) {
  while (1) {
    vTaskDelay(10000);
  }
}

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println("ESP32 Web Server");
  delay(1000);
  server.addRoute("/", handleHome);

  // Start web server with WiFi connection
  server.begin(WIFI_SSID, WIFI_PASSWORD);

  xTaskCreate(
    IntervalleTask,         // Task function
    "Tache_Relais1",       // Task name
    1024,             // Stack size (bytes)
    NULL,              // Parameters
    1,                 // Priority
    &IntervalleTaskHandle  // Task handle
  );

  xTaskCreate(
    WebTask,         // Task function
    "Tache_Serveur_Web",       // Task name
    4096,             // Stack size (bytes)
    NULL,              // Parameters
    3,                 // Priority
    &WebTaskHandle  // Task handle
  );
  time_actuel = millis();
  last_passage = time_actuel;
}

// the loop routine runs over and over again forever:
void loop() {
  sensorValue = analogRead(32);
  if (sensorValue<2500) {
    time_actuel = millis();
    vitesse = 1/((time_actuel-last_passage)/1000)*multiplicateur;
    last_passage = millis();
    Serial.println(vitesse);
    Serial.println(" ");
    while (sensorValue<2700) {
      sensorValue = analogRead(32);
    }
  }
  //Serial.println(n);
  //Serial.println(sensorValue);
}
