# Percobaan 3A: Komunikasi Data Menggunakan HTTP

**Board yang digunakan: ESP8266**

## Deskripsi Singkat
Program ini mengirimkan data sensor (suhu dan kelembaban) dalam format JSON ke endpoint
`https://httpbin.org/post` menggunakan metode HTTP POST. Karena alamat server memakai
`https://` (bukan `http://`), ada penyesuaian tambahan khusus ESP8266 dibanding kode dasar
pada modul (yang ditulis untuk ESP32).

## Kode Program Lengkap

```cpp
#include <ESP8266WiFi.h>       // Pustaka WiFi untuk board ESP8266
#include <ESP8266HTTPClient.h> // Pustaka untuk melakukan request HTTP pada ESP8266
#include <WiFiClientSecure.h>  // Pustaka untuk koneksi aman (TLS/HTTPS) pada ESP8266
#include <ArduinoJson.h>       // Pustaka untuk membuat dan mengolah data JSON

const char* ssid = "S24";
const char* password = "11111111";

const char* serverUrl = "https://httpbin.org/post"; // endpoint uji HTTP POST (HTTPS)

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password); // Memulai koneksi ke jaringan WiFi

  Serial.print("Menghubungkan ke WiFi");

  while (WiFi.status() != WL_CONNECTED) { // Tunggu sampai status WiFi terhubung
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi berhasil terhubung!");

  Serial.print("IP ESP8266: ");
  Serial.println(WiFi.localIP()); // Menampilkan IP yang didapat ESP8266
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) { // Pastikan WiFi masih terhubung sebelum kirim data

    WiFiClientSecure client; // Objek koneksi aman (dibutuhkan karena endpoint pakai HTTPS)

    client.setInsecure(); // Melewati verifikasi sertifikat SSL (cocok untuk pengujian/latihan)

    HTTPClient http;

    http.begin(client, serverUrl); // Menyiapkan koneksi HTTPS lewat objek client di atas

    http.addHeader("Content-Type", "application/json"); // Beri tahu server body berformat JSON

    JsonDocument doc;

    doc["suhu"] = 28.5;       // Contoh data suhu (°C)
    doc["kelembaban"] = 65.0; // Contoh data kelembaban (%)

    String requestBody;

    serializeJson(doc, requestBody); // Ubah objek JSON menjadi teks siap kirim

    Serial.print("Mengirim data: ");
    Serial.println(requestBody);

    int httpResponseCode = http.POST(requestBody); // Kirim data via HTTP POST

    if (httpResponseCode > 0) { // Kode > 0 berarti server merespons

      Serial.print("Kode Response HTTP: ");
      Serial.println(httpResponseCode);

      Serial.println("Isi Response:");
      Serial.println(http.getString()); // Tampilkan isi balasan dari server

    } else {

      Serial.print("Pengiriman gagal, kode error: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Tutup koneksi HTTP untuk membebaskan resource
  }

  delay(10000); // Kirim data setiap 10 detik
}
```

## Penjelasan Penyesuaian untuk ESP8266

Dibandingkan kode dasar pada modul (ditulis untuk ESP32), ada beberapa penyesuaian yang
diperlukan karena perbedaan pustaka dan cara ESP8266 menangani koneksi HTTPS:

| Baris | Penjelasan |
|---|---|
| `#include <ESP8266HTTPClient.h>` | Pengganti `HTTPClient.h` versi ESP32, memakai nama pustaka khusus ESP8266. |
| `#include <WiFiClientSecure.h>` | Pustaka tambahan yang **wajib** ada di ESP8266 untuk koneksi `https://`. Berbeda dari ESP32 yang bisa langsung `http.begin(serverUrl)`, ESP8266 butuh objek client terpisah untuk menangani lapisan keamanan TLS. |
| `WiFiClientSecure client;` | Membuat objek koneksi aman yang akan dipakai HTTPClient untuk membuka koneksi HTTPS ke server. |
| `client.setInsecure();` | Melewati proses verifikasi sertifikat SSL server. Dipakai di sini untuk mempermudah pengujian/latihan; pada aplikasi produksi sebaiknya diganti dengan verifikasi sertifikat yang sesungguhnya agar lebih aman. |
| `http.begin(client, serverUrl);` | Berbeda dari ESP32 yang cukup `http.begin(serverUrl)`, ESP8266 mengharuskan objek `client` (WiFiClientSecure) disertakan sebagai parameter pertama agar HTTPClient tahu harus memakai koneksi aman yang sudah disiapkan. |
| `Serial.print("IP ESP8266: "); Serial.println(WiFi.localIP());` | Tambahan di luar kode dasar modul, untuk memudahkan verifikasi bahwa ESP8266 sudah mendapat alamat IP yang valid dari router sebelum mulai mengirim data. |

## Cara Kerja Singkat
1. ESP8266 menyambung ke WiFi dan menampilkan IP yang didapat.
2. Setiap 10 detik, ESP8266 menyusun data suhu & kelembaban ke format JSON.
3. Data dikirim via HTTPS POST ke `httpbin.org/post` menggunakan koneksi aman (`WiFiClientSecure`).
4. Kode response dan isi balasan dari server ditampilkan di Serial Monitor sebagai bukti data diterima.

---

# Percobaan 3B: Komunikasi MQTT

**Board yang digunakan: ESP8266**

## Deskripsi Singkat
Program ini menghubungkan ESP8266 ke broker MQTT publik (`broker.hivemq.com`) dan
mempublikasikan data suhu & kelembaban dalam format JSON ke sebuah topic secara berkala.

## Kode Program Lengkap

```cpp
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
```

## Penjelasan Penyesuaian dari Kode Dasar Modul

| Baris | Penjelasan |
|---|---|
| `String clientId = "ESP8266Client-"; clientId += String(ESP.getChipId(), HEX);` | Berbeda dari kode dasar modul yang memakai angka acak (`random(0xffff)`), versi ini memakai **Chip ID** bawaan ESP8266 (`ESP.getChipId()`) sebagai identitas client. Chip ID bersifat unik dan tetap untuk tiap unit ESP8266, sehingga ID client lebih konsisten di setiap kali program dijalankan ulang, dibanding ID acak yang berubah-ubah tiap restart. |
| `bool berhasil = client.publish(mqttTopic, buffer);` | Tambahan di luar kode dasar: hasil dari `client.publish()` ditangkap ke variabel boolean untuk mengecek apakah proses publish benar-benar berhasil, bukan hanya dijalankan tanpa verifikasi hasilnya. |
| Blok `if (berhasil) { ... } else { Serial.println("Gagal mengirim data!"); }` | Penanganan tambahan: jika publish gagal (misalnya koneksi putus di tengah proses), program akan melaporkan kegagalan tersebut ke Serial Monitor alih-alih diam tanpa informasi. |

## Cara Kerja Singkat
1. ESP8266 menyambung ke WiFi, lalu menyambung ke broker MQTT publik dengan ID unik berbasis Chip ID.
2. Setiap 5 detik, ESP8266 menyusun data suhu & kelembaban ke format JSON.
3. Data dipublikasikan ke topic `wizz/tk245004/kelompokTerakhir/sensor` melalui broker.
4. Status keberhasilan publish ditampilkan di Serial Monitor, dan data bisa diverifikasi lewat aplikasi client MQTT yang subscribe ke topic yang sama.

---

# Modifikasi Program Percobaan 3A: Menambahkan Waktu (millis()) ke JSON

**Board yang digunakan: ESP8266**

## Deskripsi Singkat
Modifikasi ini menambahkan data waktu (dalam milidetik sejak ESP8266 dinyalakan) ke dalam
JSON yang dikirim melalui HTTP POST, di samping data suhu dan kelembaban yang sudah ada.

## Tujuan Modifikasi
Menjawab soal Pertanyaan Praktikum 3.5.4 nomor 4:
> "Modifikasi program agar ESP32 dapat mengirimkan data tambahan berupa waktu (dalam
> milidetik sejak dinyalakan menggunakan millis()) ke dalam JSON yang dikirim."

## Kode Program Lengkap

```cpp
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "S24";
const char* password = "11111111";

const char* serverUrl = "https://httpbin.org/post";

void setup() {

  Serial.begin(115200);

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

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    WiFiClientSecure client;

    client.setInsecure();

    HTTPClient http;

    http.begin(client, serverUrl);

    http.addHeader("Content-Type", "application/json");

    JsonDocument doc;

    doc["suhu"] = 28.5;
    doc["kelembaban"] = 65.0;

    // Ambil waktu (ms) sejak ESP8266 dinyalakan
    unsigned long waktuMillis = millis();

    // Memasukkan waktu tersebut ke JSON dengan kunci "waktu_ms"
    doc["waktu_ms"] = waktuMillis;

    String requestBody;

    serializeJson(doc, requestBody);

    Serial.print("Mengirim data: ");
    Serial.println(requestBody);

    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {

      Serial.print("Kode Response HTTP: ");
      Serial.println(httpResponseCode);

      Serial.println("Isi Response:");
      Serial.println(http.getString());

    } else {

      Serial.print("Pengiriman gagal, kode error: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }

  delay(10000);
}
```

## Penjelasan Baris Kode yang Ditambahkan

Dibandingkan kode dasar Percobaan 3A di atas, hanya dua baris baru yang ditambahkan di
dalam `loop()`, tepat setelah data suhu dan kelembaban disusun:

| Baris | Penjelasan |
|---|---|
| `unsigned long waktuMillis = millis();` | Mengambil nilai waktu (dalam milidetik) yang sudah berlalu sejak ESP8266 pertama kali dinyalakan/direset, lalu menyimpannya ke variabel `waktuMillis`. Tipe data `unsigned long` dipakai karena nilai `millis()` bisa menjadi sangat besar (hingga miliaran) seiring lamanya perangkat menyala. |
| `doc["waktu_ms"] = waktuMillis;` | Menambahkan pasangan key-value baru ke objek JSON yang sudah ada, dengan key `"waktu_ms"` dan nilai dari variabel yang baru diambil. Baris ini ditambahkan **sebelum** `serializeJson()` dipanggil, supaya data waktu ikut tersertakan saat JSON diubah menjadi teks untuk dikirim. |

Tidak ada baris lain yang perlu diubah — proses `serializeJson()`, pengiriman `http.POST()`,
maupun penanganan response tetap sama persis seperti kode dasar, karena `JsonDocument`
otomatis menyesuaikan ukuran dan isi JSON berdasarkan key-value yang ditambahkan ke
dalamnya.

## Cara Kerja Singkat
1. Setiap siklus `loop()` (tiap 10 detik), ESP8266 mengambil waktu saat ini sejak booting lewat `millis()`.
2. Nilai waktu ini dimasukkan ke objek JSON bersama data suhu dan kelembaban yang sudah ada.
3. JSON yang terkirim sekarang berbentuk tiga field, contoh: `{"suhu":28.5,"kelembaban":65.0,"waktu_ms":123456}`.
4. Response dari `httpbin.org/post` akan meng-echo kembali data tersebut, termasuk field `waktu_ms`, sebagai bukti data tambahan berhasil dikirim dan diterima server dengan benar.

## Pengujian
Upload program ini, buka Serial Monitor, dan amati apakah nilai `waktu_ms` pada data yang
dikirim terus bertambah seiring waktu di setiap siklus pengiriman (naik sekitar 10000 setiap
10 detik, sesuai jeda `delay(10000)` pada program).
