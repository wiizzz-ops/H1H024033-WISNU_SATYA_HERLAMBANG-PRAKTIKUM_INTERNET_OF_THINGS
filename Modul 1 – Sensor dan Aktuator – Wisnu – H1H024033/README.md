# Penjelasan Modifikasi Program Percobaan 1A: Akuisisi Data Sensor DHT11 (Suhu dan Kelembaban) 

## Penjelasan Penambahan Kode pada `void loop()`

### 1. Inisialisasi Variabel Penampung
```cpp
float totalSuhu = 0.0;
float totalKelembaban = 0.0;
int sampelValid = 0;
```
* `float totalSuhu = 0.0;`: Variabel untuk menampung akumulasi (penjumlahan) nilai suhu dari 5 sampel.
* `float totalKelembaban = 0.0;`: Variabel untuk menampung akumulasi nilai kelembaban dari 5 sampel.
* `int sampelValid = 0;`: Variabel penghitung (*counter*) untuk mengetahui berapa banyak sampel yang berhasil dibaca tanpa *error*.

---

### 2. Perulangan Pengambilan Sampel (`for` loop)
```cpp
for (int i = 1; i <= 5; i++) {
```
* Memulai blok perulangan *for* yang dieksekusi sebanyak 5 kali (`i = 1` sampai `i = 5`).

---

### 3. Akumulasi Data dan Validasi
```cpp
totalSuhu += temp;
totalKelembaban += hum;
sampelValid++;
```
* `totalSuhu += temp;`: Menambahkan nilai suhu sampel saat ini ke dalam total akumulasi suhu.
* `totalKelembaban += hum;`: Menambahkan nilai kelembaban sampel saat ini ke dalam total akumulasi kelembaban.
* `sampelValid++;`: Menambahkan nilai penghitung sampel valid sebanyak 1 setiap kali sensor berhasil dibaca.

---

### 4. Penundaan Antar Pengambilan Sampel
```cpp
delay(2000);
```
* Memberikan jeda waktu 2 detik di dalam *loop for* agar sesuai dengan *sampling rate* minimum sensor DHT22 (2 detik antar pengambilan sampel).

---

### 5. Penghitungan Rata-Rata
```cpp
if (sampelValid > 0) {
  float rataSuhu = totalSuhu / sampelValid;
  float rataKelembaban = totalKelembaban / sampelValid;
```
* `if (sampelValid > 0)`: Mencegah pembagian dengan nol (*division by zero*) jika seluruh 5 sampel gagal dibaca.
* `float rataSuhu = totalSuhu / sampelValid;`: Membagi total suhu dengan jumlah sampel valid untuk mendapatkan nilai rata-rata suhu.
* `float rataKelembaban = totalKelembaban / sampelValid;`: Membagi total kelembaban dengan jumlah sampel valid untuk mendapatkan nilai rata-rata kelembaban.

# Penjelasan Modifikasi Program Percobaan 2A: Kendali Aktuator Relay Berdasarkan Data Sensor 

## Penjelasan Variabel Tambahan

### 1. Inisialisasi Ambang Batas
```cpp
const float thresholdAtas = 30.0;
const float thresholdBawah = 28.0;
```
* `const float thresholdAtas = 30.0;`: Menetapkan titik batas atas suhu sebesar 30.0°C. Digunakan sebagai pemicu (*trigger*) untuk menyalakan aktuator.
* `const float thresholdBawah = 28.0;`: Menetapkan titik batas bawah suhu sebesar 28.0°C. Digunakan sebagai pemicu untuk mematikan aktuator.

### 2. Penampung Status Aktuator
```cpp
bool statusAktuator = false;
```
* `bool statusAktuator = false;`: Tipe data *boolean* (`true`/`false`) untuk menyimpan kondisi aktif/tidaknya aktuator secara perangkat lunak sebelum dikirim ke pin fisik hardware. Diinisialisasi awal bernilai `false` (OFF).

---

## Penjelasan Logika Histerisis pada `void loop()`

### 1. Pengecekan Kondisi Histerisis
```cpp
if (suhu > thresholdAtas) {
  statusAktuator = true;
} else if (suhu < thresholdBawah) {
  statusAktuator = false;
}
```
* **`if (suhu > thresholdAtas)`**: Jika pembacaan suhu dari DHT11 melebihi **30.0°C**, variabel `statusAktuator` diubah menjadi `true` (ON).
* **`else if (suhu < thresholdBawah)`**: Jika suhu turun hingga di bawah **28.0°C**, variabel `statusAktuator` diubah menjadi `false` (OFF).
* **Rentang Toleransi (28.0°C - 30.0°C)**: Apabila nilai suhu berada di antara 28.0°C dan 30.0°C, tidak ada blok `if` yang terpenuhi. Nilai `statusAktuator` akan mempertahankan kondisi terakhirnya (*hold state*), sehingga mencegah relay mati-nyala secara mendadak (*chattering*).

---

### 2. Eksekusi Perintah ke Pin Hardware
```cpp
if (statusAktuator) {
  digitalWrite(RELAYPIN, HIGH);
  Serial.println("Aktuator: ON");
} else {
  digitalWrite(RELAYPIN, LOW);
  Serial.println("Aktuator: OFF");
}
```
* Mengirimkan sinyal tegangan fisik ke `RELAYPIN` berdasarkan nilai boolean `statusAktuator`. Jika `true`, dikirim sinyal `HIGH` (Relay ON), dan jika `false`, dikirim sinyal `LOW` (Relay OFF).