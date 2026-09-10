#include <ESP8266WiFi.h>

const char* ssid = "S24";
const char* password = "11111111";

const int ledPin = 2; // LED indikator status koneksi

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Set mode WiFi menjadi Station
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Jika berhasil terhubung pada awal booting
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
  
  digitalWrite(ledPin, HIGH); // nyalakan LED sebagai indikator terhubung
}

void loop() {
  // Mengecek apakah koneksi WiFi terputus
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Status: Terhubung");
  } 
  else {
    // Bagian penanganan saat WiFi terputus
    Serial.println("Status: Terputus! Mencoba menghubungkan ulang (reconnect)...");
    digitalWrite(ledPin, LOW); // Matikan LED karena koneksi hilang
    
    // 1. Putuskan sisa koneksi yang menggantung (opsional namun direkomendasikan)
    WiFi.disconnect();
    
    // 2. Perintahkan ESP32 untuk menghubungkan ulang
    WiFi.begin(ssid, password);
    
    // 3. Tunggu proses koneksi ulang dengan batas waktu (timeout)
    int waktuTunggu = 0;
    while (WiFi.status() != WL_CONNECTED && waktuTunggu < 10) {
      delay(500);
      Serial.print(".");
      waktuTunggu++;
    }
    Serial.println();
    
    // 4. Evaluasi hasil percobaan reconnect
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Berhasil terhubung kembali!");
      Serial.print("IP Address : ");
      Serial.println(WiFi.localIP());
      digitalWrite(ledPin, HIGH); // Nyalakan LED kembali
    } else {
      Serial.println("Gagal menghubungkan ulang. Akan mencoba lagi pada siklus berikutnya.");
    }
  }
  
  // Jeda 5 detik sebelum melakukan pengecekan status berikutnya
  delay(5000);
}