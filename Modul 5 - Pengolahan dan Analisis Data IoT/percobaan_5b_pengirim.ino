#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Konfigurasi WiFi & MQTT
const char* ssid = "Spiderfoot";
const char* password = "11111111";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

const char* topicData = "IoT/klmpk0/data";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000; // publish data setiap 5 detik

// Fungsi simulasi suhu dengan pola gelombang, noise, dan anomali
float bacaSuhuMock() {
  static unsigned long jumlahBaca = 0;
  jumlahBaca++;

  // Suhu dasar berubah secara bertahap
  float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);

  // Noise kecil untuk mensimulasikan fluktuasi sensor
  float noise = random(-5, 6) * 0.1;

  float suhu = suhuDasar + noise;

  // Lonjakan suhu setiap 25 pembacaan untuk simulasi anomali
  if (jumlahBaca % 25 == 0) {
    suhu += 8.0;
  }

  return suhu;
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Terhubung ke MQTT broker");
    } else {
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
  
  client.loop(); // Memproses tugas latar belakang MQTT
  
  // Publish data dummy secara berkala
  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();
    
    // Mengambil data dari fungsi mock
    float suhuDummy = bacaSuhuMock(); 
    
    JsonDocument doc;
    doc["suhu"] = suhuDummy;
    
    char buffer[128];
    serializeJson(doc, buffer);
    client.publish(topicData, buffer);
    
    Serial.print("Data terkirim: ");
    Serial.println(buffer);
  }
}