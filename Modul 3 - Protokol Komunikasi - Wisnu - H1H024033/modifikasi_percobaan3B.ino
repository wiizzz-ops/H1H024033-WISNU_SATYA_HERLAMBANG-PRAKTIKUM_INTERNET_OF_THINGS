#include <ESP8266WiFi.h>   // Pustaka WiFi untuk board ESP8266
#include <PubSubClient.h>  // Pustaka untuk komunikasi protokol MQTT
#include <ArduinoJson.h>   // Pustaka untuk membuat dan mengolah data JSON

const char* ssid = "S24";
const char* password = "11111111";

const char* mqttServer = "broker.hivemq.com";               // Alamat broker MQTT publik
const int mqttPort = 1883;                                   // Port default MQTT (tanpa TLS)
const char* mqttTopic = "wizz/tk245004/kelompokTerakhir/sensor"; // Topic unik untuk publish data

WiFiClient espClient;          // Objek koneksi TCP dasar
PubSubClient client(espClient); // Objek client MQTT yang berjalan di atas koneksi TCP tadi

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

  while (!client.connected()) { // Ulangi sampai berhasil terhubung ke broker

    Serial.print("Menghubungkan ke broker MQTT...");

    String clientId = "ESP8266Client-";
    clientId += String(ESP.getChipId(), HEX); // ID unik diambil dari Chip ID ESP8266

    if (client.connect(clientId.c_str())) {

      Serial.println("berhasil terhubung!");

    } else {

      Serial.print("gagal, rc=");
      Serial.print(client.state()); // Menampilkan kode error koneksi MQTT
      Serial.println(" coba lagi dalam 2 detik");

      delay(2000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  hubungkanWiFi();

  client.setServer(mqttServer, mqttPort); // Mendaftarkan alamat & port broker yang dituju
}

void loop() {

  if (!client.connected()) { // Jika koneksi ke broker terputus, coba sambung ulang
    hubungkanMQTT();
  }

  client.loop(); // Wajib dipanggil terus-menerus untuk menjaga koneksi & memproses pesan MQTT

  JsonDocument doc;

  doc["suhu"] = 28.5;
  doc["kelembaban"] = 65.0;

  char buffer[128];

  serializeJson(doc, buffer); // Serialisasi JSON ke buffer karakter (dibutuhkan PubSubClient)

  bool berhasil = client.publish(mqttTopic, buffer); // Mempublikasikan data, true jika berhasil

  if (berhasil) {
    Serial.print("Data terkirim ke topic ");
    Serial.print(mqttTopic);
    Serial.print(": ");
    Serial.println(buffer);
  } else {
    Serial.println("Gagal mengirim data!");
  }

  delay(5000); // Publish data setiap 5 detik
}