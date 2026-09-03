#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("Memulai program akuisisi data dengan rata-rata 5 pembacaan...");
}

void loop() {
  float totalSuhu = 0.0;
  float totalKelembaban = 0.0;
  int sampelValid = 0;

  // Perulangan untuk mengambil 5 kali sampel data
  for (int i = 1; i <= 5; i++) {
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();

    // Memeriksa validitas sampel
    if (isnan(hum) || isnan(temp)) {
      Serial.print("Sampel ke-");
      Serial.print(i);
      Serial.println(": Gagal membaca data dari sensor!");
    } else {
      totalSuhu += temp;
      totalKelembaban += hum;
      sampelValid++;

      Serial.print("Sampel ke-");
      Serial.print(i);
      Serial.print(" -> Suhu: ");
      Serial.print(temp);
      Serial.print(" °C, Kelembaban: ");
      Serial.print(hum);
      Serial.println("%");
    }

    delay(2000); // Jeda 2 detik antar sampel
  }

  // Kalkulasi dan penampil rata-rata
  if (sampelValid > 0) {
    float rataSuhu = totalSuhu / sampelValid;
    float rataKelembaban = totalKelembaban / sampelValid;

    Serial.println("------------------------------------------");
    Serial.print("RATA-RATA (dari ");
    Serial.print(sampelValid);
    Serial.println(" sampel valid):");
    Serial.print("Rata-rata Suhu       : ");
    Serial.print(rataSuhu);
    Serial.println(" °C");
    Serial.print("Rata-rata Kelembaban : ");
    Serial.print(rataKelembaban);
    Serial.println(" %");
    Serial.println("------------------------------------------\n");
  } else {
    Serial.println("Error: Tidak ada sampel valid untuk dihitung rata-ratanya!\n");
  }
}