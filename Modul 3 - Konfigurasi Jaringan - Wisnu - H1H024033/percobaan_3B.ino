#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "S24";
const char* password = "11111111";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttTopic = "wizz/tk245004/kelompokTerakhir/sensor";

WiFiClient espClient;
PubSubClient client(espClient);

void hubungkanWiFi() {

  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi berhasil terhubung!");

  Serial.print("IP ESP8266: ");
  Serial.println(WiFi.localIP());
}

void hubungkanMQTT() {

  while (!client.connected()) {

    Serial.print("Menghubungkan ke broker MQTT...");

    String clientId = "ESP8266Client-";
    clientId += String(ESP.getChipId(), HEX);

    if (client.connect(clientId.c_str())) {

      Serial.println("berhasil terhubung!");

    } else {

      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");

      delay(2000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  hubungkanWiFi();

  client.setServer(mqttServer, mqttPort);
}

void loop() {

  if (!client.connected()) {
    hubungkanMQTT();
  }

  client.loop();

  JsonDocument doc;

  doc["suhu"] = 28.5;
  doc["kelembaban"] = 65.0;

  char buffer[128];

  serializeJson(doc, buffer);

  bool berhasil = client.publish(mqttTopic, buffer);

  if (berhasil) {
    Serial.print("Data terkirim ke topic ");
    Serial.print(mqttTopic);
    Serial.print(": ");
    Serial.println(buffer);
  } else {
    Serial.println("Gagal mengirim data!");
  }

  delay(5000);
}