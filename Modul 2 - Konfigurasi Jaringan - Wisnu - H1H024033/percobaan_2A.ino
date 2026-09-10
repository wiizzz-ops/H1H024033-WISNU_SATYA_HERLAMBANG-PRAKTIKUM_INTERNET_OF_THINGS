#include <ESP8266WiFi.h>

// ===============================
// Konfigurasi WiFi
// ===============================
const char* ssid = "S24";
const char* password = "pppppppp";

// LED indikator
// GPIO 2 pada ESP8266 = D4 pada NodeMCU
#define LED_PIN 2

void setup() {
  // Memulai komunikasi Serial
  Serial.begin(115200);

  // Mengatur pin LED sebagai output
  pinMode(LED_PIN, OUTPUT);

  // LED mati saat awal
  digitalWrite(LED_PIN, LOW);

  // Mengatur ESP8266 sebagai Station
  WiFi.mode(WIFI_STA);

  // Memulai koneksi WiFi
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Menghubungkan ke WiFi");

  // Menunggu sampai ESP8266 terhubung
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // WiFi berhasil terhubung
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");

  // Menyalakan LED indikator
  digitalWrite(LED_PIN, HIGH);

  // Menampilkan informasi jaringan
  Serial.print("SSID        : ");
  Serial.println(WiFi.SSID());

  Serial.print("IP Address  : ");
  Serial.println(WiFi.localIP());

  Serial.print("MAC Address : ");
  Serial.println(WiFi.macAddress());

  Serial.print("RSSI        : ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

void loop() {
}