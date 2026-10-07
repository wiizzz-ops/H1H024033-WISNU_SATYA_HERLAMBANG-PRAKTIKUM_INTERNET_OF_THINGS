// Mengimpor library DHT untuk membaca sensor suhu dan kelembaban
#include <DHT.h>

#define DHTPIN 4       // pin data DHT11 terhubung ke GPIO 4
#define DHTTYPE DHT11  // tipe sensor yang digunakan
#define RELAYPIN D1    // pin kendali relay

// Membuat objek sensor DHT dengan pin dan tipe yang sudah didefinisikan
DHT dht(DHTPIN, DHTTYPE);

const float suhuThreshold = 30.0; // ambang batas suhu

void setup() {
  Serial.begin(115200);   // inisialisasi komunikasi serial dengan baud rate 115200
  dht.begin();            // inisialisasi sensor DHT11

  pinMode(RELAYPIN, OUTPUT);           // mengatur pin relay sebagai OUTPUT
  digitalWrite(RELAYPIN, LOW);         // aktuator mati di awal

  Serial.println("Memulai akuisisi data sensor DHT11..."); // pesan awal ke Serial Monitor
}

void loop() {
  float suhu = dht.readTemperature();  // membaca nilai suhu dari sensor

  // mengecek apakah data suhu valid (isnan = true jika pembacaan gagal)
  if (isnan(suhu)) {
    Serial.println("Gagal membaca data dari sensor DHT11!"); // tampilkan pesan error
  } else {
    // menampilkan nilai suhu ke Serial Monitor
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C -> ");

    if (suhu > suhuThreshold) {
      digitalWrite(RELAYPIN, HIGH);
      Serial.println("Aktuator: ON");
    } else {
      digitalWrite(RELAYPIN, LOW);    // matikan relay/LED jika suhu di bawah threshold
      Serial.println("Aktuator: OFF");
    }
  }

  delay(2000);
}