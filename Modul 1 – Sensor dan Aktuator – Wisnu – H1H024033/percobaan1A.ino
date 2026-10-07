// Mengimpor library DHT untuk membaca sensor suhu dan kelembaban
#include <DHT.h>

#define DHTPIN 4       // Mendefinisikan pin data sensor DHT11 terhubung ke GPIO 4
#define DHTTYPE DHT11  // Mendefinisikan tipe sensor yang digunakan adalah DHT11

// Membuat objek dht dengan pin dan tipe sensor yang sudah didefinisikan
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200); // Menginisialisasi komunikasi serial dengan baud rate 115200
  dht.begin();          // Menginisialisasi sensor DHT11
  
  // Menampilkan pesan awal ke Serial Monitor
  Serial.println("Memulai akuisisi data sensor DHT11...");
}

void loop() {
  float kelembaban = dht.readHumidity();     // Membaca nilai kelembaban dari sensor
  float suhu = dht.readTemperature();        // Membaca nilai suhu dari sensor

  // Mengecek apakah data yang dibaca valid
  if (isnan(kelembaban) || isnan(suhu)) {
    Serial.println("Gagal membaca data dari sensor DHT11!"); // Menampilkan pesan error jika pembacaan gagal
  } else {
    // Menampilkan nilai suhu dan kelembaban di Serial Monitor
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C, Kelembaban: ");
    Serial.print(kelembaban);
    Serial.println(" %");
  }

  delay(2000);
}