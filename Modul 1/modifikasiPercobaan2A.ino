#include <DHT.h>

#define DHTPIN 4       // pin data DHT11 terhubung ke GPIO 4
#define DHTTYPE DHT11  // tipe sensor yang digunakan adalah DHT11
#define RELAYPIN D1    // pin kendali relay

DHT dht(DHTPIN, DHTTYPE);

// Menentukan dua ambang batas (Histerisis)
const float thresholdAtas = 30.0;  // Suhu batas untuk menyalakan aktuator
const float thresholdBawah = 28.0; // Suhu batas untuk mematikan aktuator

// Variabel penampung status aktuator (false = OFF, true = ON)
bool statusAktuator = false;

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW); // Aktuator mati di awal

  Serial.println("Memulai program kendali histerisis sensor DHT11...");
}

void loop() {
  float suhu = dht.readTemperature(); // Membaca data suhu dari sensor DHT11

  if (isnan(suhu)) {
    Serial.println("Gagal membaca data dari sensor DHT11!");
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C -> ");

    // Logika Histerisis
    if (suhu > thresholdAtas) {
      statusAktuator = true;  // Menyala jika suhu naik melewati 30.0°C
    } else if (suhu < thresholdBawah) {
      statusAktuator = false; // Baru mati jika suhu turun di bawah 28.0°C
    }
    // Catatan: Jika suhu di antara 28.0°C - 30.0°C, statusAktuator tidak berubah

    // Eksekusi kendali fisik ke pin relay
    if (statusAktuator) {
      digitalWrite(RELAYPIN, HIGH);
      Serial.println("Aktuator: ON");
    } else {
      digitalWrite(RELAYPIN, LOW);
      Serial.println("Aktuator: OFF");
    }
  }

  delay(2000); // Jeda pembacaan setiap 2 detik
}