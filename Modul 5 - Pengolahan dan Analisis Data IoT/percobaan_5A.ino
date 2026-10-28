const int N = 5;          // jumlah sampel moving average
float buffer[N];          // menyimpan N pembacaan terakhir
int indeks = 0;           // posisi penulisan berikutnya pada buffer
int jumlah = 0;           // jumlah sampel yang sudah terisi

float suhuMin = 1000.0;
float suhuMax = -1000.0;

// Fungsi moving average (ditambahkan kembali agar kode bisa dicompile)
float hitungMovingAverage(float nilaiBaru) {
  buffer[indeks] = nilaiBaru;
  indeks = (indeks + 1) % N;
  
  if (jumlah < N) {
    jumlah++;
  }

  float total = 0;
  for (int i = 0; i < jumlah; i++) {
    total += buffer[i];
  }
  
  return total / jumlah;
}

// Fungsi menghitung simpangan baku (standard deviation) dari data pada buffer
float hitungSimpanganBaku() {
  if (jumlah < 2) return 0.0; // butuh minimal 2 data

  // Hitung rata-rata
  float total = 0;
  for (int i = 0; i < jumlah; i++) {
    total += buffer[i];
  }
  float rerata = total / jumlah;

  // Hitung jumlah kuadrat selisih dari rata-rata
  float sumKuadrat = 0;
  for (int i = 0; i < jumlah; i++) {
    float selisih = buffer[i] - rerata;
    sumKuadrat += selisih * selisih;
  }

  // Simpangan baku populasi (dibagi N)
  return sqrt(sumKuadrat / jumlah);
}

float bacaSuhuMock() {
  static unsigned long jumlahBaca = 0;
  jumlahBaca++;

  // Suhu dasar berubah secara bertahap (Perbaikan: ditambahkan operator '*' sebelum sin)
  float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);

  // Noise kecil untuk mensimulasikan fluktuasi sensor
  float noise = random(-5, 6) * 0.1;

  float suhu = suhuDasar + noise;

  // Lonjakan suhu setiap 25 pembacaan untuk simulasi anomali
  if (jumlahBaca % 25 == 0) {
    suhu += 8.0;
  }

  return suhu;
}

void setup() {
  Serial.begin(115200);
  // dht.begin(); // Dihapus karena tidak lagi menggunakan DHT
}

void loop() {
  // Menggunakan data simulasi dari bacaSuhuMock()
  float suhu = bacaSuhuMock();

  if (isnan(suhu)) {
    Serial.println("Pembacaan gagal, data diabaikan");
  } else {
    float suhuFilter = hitungMovingAverage(suhu);

    if (suhu < suhuMin) suhuMin = suhu;
    if (suhu > suhuMax) suhuMax = suhu;

    Serial.print("Mentah: ");
    Serial.print(suhu);
    Serial.print(" | Filter: ");
    Serial.print(suhuFilter);
    Serial.print(" | Min: ");
    Serial.print(suhuMin);
    Serial.print(" | Max: ");
    Serial.print(suhuMax);
    Serial.print(" | StdDev: ");
    Serial.println(hitungSimpanganBaku());
  }

  delay(2000);
}