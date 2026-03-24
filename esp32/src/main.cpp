#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>       // Dodane dla NTP
#include <sys/time.h>   // Dodane dla precyzyjnych ms
#include "secrets.h"

// Deklaracje funkcji z pliku sensorism.cpp
float getsimAzimuth();


WiFiClient espClient;
PubSubClient mqttClient(espClient);
String deviceId;

String generateDeviceIdFromEfuse() {
    uint64_t chipId = ESP.getEfuseMac();
    char id[32];
    snprintf(id, sizeof(id), "esp32-%04X%08X",
             (uint16_t)(chipId >> 32),
             (uint32_t)chipId);
    return String(id);
}

void connectWiFi() {
    Serial.print("Laczenie z Wi-Fi: ");
    Serial.println(WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nPolaczono z Wi-Fi");
    Serial.print("Adres IP: ");
    Serial.println(WiFi.localIP());

    // --- Synchronizacja czasu NTP ---
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    struct tm timeinfo;
    Serial.print("Oczekiwanie na synchronizacje czasu...");
    while (!getLocalTime(&timeinfo)) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nCzas zsynchronizowany.");
}

void connectMQTT() {
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    while (!mqttClient.connected()) {
        Serial.print("Laczenie z MQTT...");
        if (mqttClient.connect(deviceId.c_str())) {
            Serial.println("OK");
        } else {
            Serial.print("blad, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" - ponowna proba za 2 s");
            delay(2000);
        }
    }
}

// Funkcja pobierająca aktualny czas Unix w milisekundach
long long getTimestampMs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((long long)tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

// Ulepszona funkcja publikująca
void publishMeasurement(String sensorName, float value, String unit) {
    StaticJsonDocument<256> doc;
    doc["device_id"] = deviceId;
    doc["sensor"] = sensorName;   
    doc["value"] = value;         
    doc["unit"] = unit;
    
    // ZMIANA: używamy prawdziwego czasu Unix zamiast millis()
    doc["ts_ms"] = getTimestampMs();

    char payload[256];
    serializeJson(doc, payload);

    String currentTopic = "lab/" + String(MQTT_GROUP) + "/" + deviceId + "/" + sensorName;

    mqttClient.publish(currentTopic.c_str(), payload);
    Serial.print("Publikacja na topic: ");
    Serial.println(currentTopic);
    Serial.println(payload);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    deviceId = generateDeviceIdFromEfuse();
    
    Serial.print("Device ID: ");
    Serial.println(deviceId);
    
    connectWiFi();
    connectMQTT();
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
    }
    if (!mqttClient.connected()) {
        connectMQTT();
    }
    mqttClient.loop();

    // Wrzucamy urozmaicenie! Wysyłamy kolejno 3 wiadomości:
    publishMeasurement("temperature", temperatureRead(), "C");             
    publishMeasurement("azimuth", getsimAzimuth(), "deg");    
       

    delay(5000);
}