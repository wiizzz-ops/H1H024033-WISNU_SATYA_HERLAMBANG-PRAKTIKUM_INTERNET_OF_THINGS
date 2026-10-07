#include <ESP8266WiFi.h>   // Pustaka WiFi untuk board ESP8266
#include <PubSubClient.h>  // Pustaka untuk komunikasi protokol MQTT
#include <ArduinoJson.h>   // Pustaka untuk membuat dan mengolah data JSON

const char* ssid = "S24";
const char* password = "11111111";
const char* mqttServer = "broker.hivemq.com";          // Alamat broker MQTT publik
const int mqttPort = 1883;                               // Port default MQTT (tanpa TLS)
const char* topicPerintah = "unsoed/tk245004/panas";    // Topic yang di-subscribe
const int ledPin = 5;                                    // GPIO5, berlabel D1 pada NodeMCU ESP8266

WiFiClient espClient;          // Objek koneksi TCP dasar
PubSubClient client(espClient); // Objek client MQTT yang berjalan di atas koneksi TCP tadi

// Fungsi callback dipanggil otomatis setiap ada pesan baru masuk
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];  // Menggabungkan payload byte demi byte menjadi String
  }

  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  // Deserialisasi data JSON yang diterima
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);

  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return; // Hentikan proses jika JSON tidak valid
  }

  const char* perintah = doc["perintah"]; // Mengambil nilai dari key "perintah"

  if (String(perintah) == "ON") {
    digitalWrite(ledPin, HIGH);
    Serial.println("Aktuator: ON");
  } else if (String(perintah) == "OFF") {
    digitalWrite(ledPin, LOW);
    Serial.println("Aktuator: OFF");
  }
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
    Serial.print("Menghubungkan ke broker MQTT...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX); // ID unik acak

    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
      client.subscribe(topicPerintah); // subscribe setelah berhasil terhubung
      Serial.print("Subscribe ke topic: ");
      Serial.println(topicPerintah);
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
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Pastikan LED mati di awal

  hubungkanWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback); // daftarkan fungsi callback
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop(); // wajib dipanggil terus-menerus agar pesan dapat diterima
}