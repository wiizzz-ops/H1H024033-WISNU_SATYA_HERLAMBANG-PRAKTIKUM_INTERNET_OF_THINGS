# Modifikasi Program Percobaan 2A: Konfigurasi Mode Station (STA)

```cpp
#include <ESP8266WiFi.h>  // Pustaka WiFi resmi untuk ESP8266

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
```

## Penjelasan Baris Kode yang Ditambahkan/Dimodifikasi

Dibandingkan program dasar Percobaan 2A pada modul, bagian `else` di dalam `loop()`
diperluas. Berikut penjelasan tiap bagian baru:

| Baris | Penjelasan |
|---|---|
| `Serial.println("Status: Terputus! Mencoba menghubungkan ulang (reconnect)...");` | Memberi tahu pengguna lewat Serial Monitor bahwa program sedang masuk ke mode reconnect, bukan hanya diam. |
| `WiFi.disconnect();` | Memutus sisa sesi koneksi lama yang mungkin masih menggantung di modul WiFi ESP32, supaya proses `WiFi.begin()` berikutnya dimulai dari kondisi bersih dan tidak konflik. |
| `WiFi.begin(ssid, password);` | Memerintahkan ESP32 untuk memulai ulang proses asosiasi ke jaringan WiFi yang sama menggunakan kredensial yang sudah tersimpan. |
| `int waktuTunggu = 0;` | Variabel counter untuk membatasi berapa lama program menunggu proses reconnect, supaya `loop()` tidak macet selamanya jika WiFi tidak kunjung tersedia. |
| `while (WiFi.status() != WL_CONNECTED && waktuTunggu < 10)` | Loop tunggu dengan **timeout**: mencoba maksimal 10 x 500ms = 5 detik. Berbeda dari `while` di `setup()` yang tidak berbatas waktu — ini penting supaya `loop()` tetap berjalan periodik meskipun reconnect gagal. |
| `delay(500); Serial.print("."); waktuTunggu++;` | Jeda antar percobaan, indikator progres visual, dan penambahan counter agar loop timeout berakhir. |
| Blok `if (WiFi.status() == WL_CONNECTED) { ... } else { ... }` setelah loop tunggu | Mengevaluasi hasil percobaan reconnect: jika berhasil, tampilkan IP baru dan nyalakan LED kembali; jika gagal, beri tahu pengguna bahwa program akan mencoba lagi pada siklus `loop()` berikutnya (5 detik kemudian). |

## Cara Kerja Singkat
1. Setiap 5 detik, ESP32 mengecek status koneksi WiFi.
2. Jika terhubung, program hanya melaporkan status seperti biasa.
3. Jika terputus, ESP32 otomatis mencoba reconnect dengan batas waktu 5 detik.
4. Hasil reconnect (berhasil/gagal) dilaporkan ke Serial Monitor, dan LED menyala kembali jika berhasil.
5. Jika masih gagal, ESP32 tidak berhenti — ia akan mencoba lagi pada siklus `loop()` berikutnya.

---

# Modifikasi Program Percobaan 2B: Konfigurasi Mode Access Point (AP)

```cpp
#include <ESP8266WiFi.h>  // Pustaka WiFi resmi untuk ESP8266

// Kredensial untuk mode Station (menyambung ke WiFi rumah)
const char* sta_ssid     = "NAMA_WIFI_ANDA";
const char* sta_password = "PASSWORD_WIFI_ANDA";

// Kredensial untuk mode Access Point (dibuat oleh ESP32)
const char* ap_ssid     = "ESP32_AccessPoint";
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
```

## Penjelasan Baris Kode yang Ditambahkan/Dimodifikasi

Dibandingkan program dasar Percobaan 2B pada modul (yang hanya menjalankan
`WiFi.mode(WIFI_AP)`), berikut perubahan dan tambahannya:

| Baris | Penjelasan |
|---|---|
| `const char* sta_ssid`, `sta_password` | Variabel kredensial tambahan untuk menyambung ke WiFi rumah sebagai Station, terpisah dari kredensial AP agar tidak tertukar. |
| `WiFi.mode(WIFI_AP_STA);` | Perubahan inti: mengganti `WIFI_AP` menjadi `WIFI_AP_STA`, sehingga modul WiFi ESP32 mengaktifkan dua peran sekaligus — sebagai host (AP) dan klien (STA) — pada radio yang sama. |
| `WiFi.softAP(ap_ssid, ap_password);` dst. | Bagian ini tetap sama seperti kode dasar, hanya dipindah ke dalam blok "Bagian Access Point" agar lebih terstruktur. |
| `WiFi.begin(sta_ssid, sta_password);` | Baris baru: memulai proses koneksi ke jaringan WiFi rumah, persis seperti pada mode STA di Percobaan 2A. |
| `int waktuTunggu = 0; while (... && waktuTunggu < 20)` | Loop tunggu dengan timeout (maks. 10 detik) untuk proses koneksi STA. Diberi batas waktu karena AP harus tetap bisa jalan meski koneksi STA gagal — program tidak boleh macet menunggu STA tanpa batas. |
| Blok `if (WiFi.status() == WL_CONNECTED) { ... } else { ... }` di `setup()` | Melaporkan hasil koneksi STA: jika berhasil, tampilkan IP yang didapat dari router rumah; jika gagal, program tetap lanjut karena AP sudah aktif duluan. |
| Bagian tambahan di `loop()` untuk status STA | Selain memantau jumlah client AP (seperti kode dasar), ditambahkan pemantauan status STA setiap 5 detik supaya pengguna bisa melihat kedua peran (AP dan STA) berjalan bersamaan dari satu Serial Monitor. |

## Cara Kerja Singkat
1. Saat booting, ESP32 langsung mengaktifkan AP (`ESP32_AccessPoint`) — perangkat lain
   bisa langsung menyambung ke sini kapan saja.
2. Secara paralel, ESP32 mencoba menyambung sebagai klien ke WiFi rumah.
3. Kedua mode berjalan bersamaan: ESP32 punya dua alamat IP sekaligus —
   IP AP (`192.168.4.1`, untuk perangkat yang konek ke AP-nya) dan IP STA
   (didapat dari router rumah, untuk akses internet/jaringan lokal).
4. `loop()` terus memantau dan melaporkan status kedua peran setiap 5 detik.

