#include <ESP8266WiFi.h>  // Pustaka WiFi resmi untuk ESP8266

// Kredensial untuk mode Station (menyambung ke WiFi rumah)
const char* sta_ssid     = "S24";
const char* sta_password = "11111111";

// Kredensial untuk mode Access Point (dibuat oleh ESP32)
const char* ap_ssid     = "ESP8266 CIIDUK WARRIOR";
const char* ap_password = "12345678"; // minimal 8 karakter

void setup() {
  Serial.begin(115200);

  // Set mode WiFi menjadi gabungan AP + STA
  WiFi.mode(WIFI_AP_STA);

  // --- Bagian Access Point ---
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress apIP = WiFi.softAPIP();
  Serial.println("Access Point aktif!");
  Serial.print("AP SSID       : ");
  Serial.println(ap_ssid);
  Serial.print("AP IP Address : ");
  Serial.println(apIP);

  // --- Bagian Station ---
  WiFi.begin(sta_ssid, sta_password);
  Serial.print("Menghubungkan ke WiFi rumah");
  int waktuTunggu = 0;
  while (WiFi.status() != WL_CONNECTED && waktuTunggu < 20) {
    delay(500);
    Serial.print(".");
    waktuTunggu++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("STA berhasil terhubung ke WiFi rumah!");
    Serial.print("STA IP Address : ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("STA gagal terhubung ke WiFi rumah (AP tetap aktif).");
  }
}

void loop() {
  // Memantau jumlah perangkat yang terhubung ke Access Point ESP32
  int jumlahClient = WiFi.softAPgetStationNum();
  Serial.print("Jumlah perangkat di AP  : ");
  Serial.println(jumlahClient);

  // Memantau status koneksi Station ke WiFi rumah
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Status STA              : Terhubung, IP = ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Status STA              : Terputus dari WiFi rumah");
  }

  delay(5000);
}