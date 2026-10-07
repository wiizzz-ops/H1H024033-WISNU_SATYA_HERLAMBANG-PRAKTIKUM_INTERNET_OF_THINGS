# Percobaan 4A: Subscribe dan Deserialisasi Data JSON untuk Kendali Aktuator

**Board yang digunakan: ESP8266**

## Deskripsi Singkat
Program ini membuat ESP8266 melakukan **subscribe** ke sebuah topic MQTT dan menunggu
pesan perintah masuk. Setiap kali ada pesan baru, fungsi `callback()` otomatis dipanggil
untuk mem-parsing (deserialisasi) data JSON dari pesan tersebut, lalu menyalakan/mematikan
LED sesuai nilai `"perintah"` yang diterima.

## Kode Program Lengkap

```cpp
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
```

## Penjelasan Penyesuaian dari Kode Dasar Modul

| Baris | Penjelasan |
|---|---|
| `#include <ESP8266WiFi.h>` | Pengganti `WiFi.h` versi ESP32, memakai nama pustaka khusus ESP8266. |
| `const char* topicPerintah = "unsoed/tk245004/panas";` | Topic disesuaikan menjadi nama unik milik kelompok, menggantikan placeholder `"...kelompokAnda/perintah"` pada kode dasar modul. |
| `const int ledPin = 5;` | Disesuaikan ke GPIO5 (berlabel **D1** pada board NodeMCU ESP8266), menggantikan `26` pada kode dasar yang ditulis untuk ESP32 (GPIO26 tidak tersedia di ESP8266). |

Selebihnya, struktur logika program (fungsi `callback()`, `hubungkanWiFi()`, `hubungkanMQTT()`,
`setup()`, dan `loop()`) sama persis dengan kode dasar pada modul.

## Cara Kerja Singkat
1. ESP8266 menyambung ke WiFi, lalu ke broker MQTT dan melakukan subscribe ke topic perintah.
2. Program menunggu secara pasif (`client.loop()`) hingga ada pesan baru masuk pada topic tersebut.
3. Saat pesan diterima, fungsi `callback()` otomatis dipanggil, men-deserialisasi isi JSON-nya, lalu memeriksa nilai `"perintah"`.
4. LED di pin **D1 (GPIO5)** dinyalakan jika perintah `"ON"`, dan dimatikan jika perintah `"OFF"`.

---

# Percobaan 4B: Pertukaran Data Dua Arah (Publish dan Subscribe Secara Bersamaan)

**Board yang digunakan: ESP8266**

## Deskripsi Singkat
Program ini menggabungkan proses **publish** (mengirim data suhu dari sensor DHT11 secara
berkala) dan **subscribe** (menerima perintah kendali LED) secara bersamaan dalam satu
program, tanpa saling memblokir — menggunakan pendekatan non-blocking berbasis `millis()`.

## Kode Program Lengkap

```cpp
#include <ESP8266WiFi.h>   // Pustaka WiFi untuk board ESP8266
#include <PubSubClient.h>  // Pustaka untuk komunikasi protokol MQTT
#include <ArduinoJson.h>   // Pustaka untuk membuat dan mengolah data JSON
#include <DHT.h>           // Pustaka untuk membaca sensor DHT

const char* ssid = "S24";
const char* password = "11111111";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

const char* topicData = "unsoed/panas/data";         // Topic untuk publish data suhu
const char* topicPerintah = "unsoed/panas/perintah"; // Topic untuk subscribe perintah

#define DHTPIN 4        // GPIO4, berlabel D2 pada NodeMCU ESP8266
#define DHTTYPE DHT11

const int ledPin = 5;   // GPIO5, berlabel D1 pada NodeMCU ESP8266

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000; // publish data setiap 5 detik (non-blocking)

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  JsonDocument doc;
  if (deserializeJson(doc, pesan)) return; // abaikan jika parsing gagal

  const char* perintah = doc["perintah"];
  digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);

  Serial.print("Perintah diterima -> Aktuator: ");
  Serial.println(perintah);
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
      client.subscribe(topicPerintah);
      Serial.println("Terhubung dan subscribe topic perintah");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  dht.begin(); // Inisialisasi sensor DHT11

  hubungkanWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }

  client.loop(); // memproses pesan masuk secara terus-menerus

  // Publish data sensor secara berkala tanpa memblokir proses subscribe
  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();
    float suhu = dht.readTemperature();

    if (!isnan(suhu)) {
      JsonDocument doc;
      doc["suhu"] = suhu;

      char buffer[128];
      serializeJson(doc, buffer);
      client.publish(topicData, buffer);

      Serial.print("Data terkirim: ");
      Serial.println(buffer);
    } else {
      Serial.println("Gagal membaca sensor DHT!");
    }
  }
}
```

## Penjelasan Penyesuaian dari Kode Dasar Modul

| Baris | Penjelasan |
|---|---|
| `#include <ESP8266WiFi.h>` | Pengganti `WiFi.h` versi ESP32. |
| `const char* topicData`, `topicPerintah` | Disesuaikan menjadi nama topic unik milik kelompok, menggantikan placeholder pada kode dasar modul. |
| `const int ledPin = 5;` | Disesuaikan ke GPIO5 (D1) untuk ESP8266, menggantikan `26` pada kode dasar (ditulis untuk ESP32). |
| `#define DHTPIN 4` | Tidak diubah dari kode dasar modul — GPIO4 (berlabel **D2** pada ESP8266) tersedia juga di ESP8266, sehingga angka pin ini tetap sama meski board berbeda. |
| `else { Serial.println("Gagal membaca sensor DHT!"); }` | Tambahan di luar kode dasar modul: jika `isnan(suhu)` bernilai benar (pembacaan sensor gagal), program akan melaporkan kegagalan tersebut ke Serial Monitor, bukan diam tanpa informasi. |

## Cara Kerja Singkat
1. ESP8266 menyambung ke WiFi dan broker MQTT, lalu subscribe ke topic perintah.
2. Setiap iterasi `loop()`, `client.loop()` selalu dipanggil agar pesan masuk bisa langsung diproses kapan saja (tidak terhambat proses lain).
3. Program mengecek selisih `millis()` terhadap waktu publish terakhir; jika sudah melewati `intervalPublish` (5 detik), data suhu dari DHT11 dibaca dan dipublikasikan ke topic data.
4. Jika pesan perintah MQTT diterima di waktu manapun (meski sedang dalam proses publish), fungsi `callback()` tetap langsung dijalankan untuk mengubah status LED — inilah yang membuat sistem bersifat **full duplex** tanpa saling mengganggu.
