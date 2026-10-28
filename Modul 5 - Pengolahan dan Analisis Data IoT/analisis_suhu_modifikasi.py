import pandas as pd
import matplotlib.pyplot as plt

JUMLAH_DATA = 5  # ukuran jendela moving average
AMBANG_Z = 2  # batas z-score untuk anomali


def hitung_zscore(series):
    rata2 = series.mean()
    std = series.std()
    if std == 0 or pd.isna(std):
        return pd.Series(0.0, index=series.index), rata2, std
    return (series - rata2) / std, rata2, std


# --- Tahap 1: baca data hasil CSV ---
df = pd.read_csv("data_suhu.csv")
print(f"Jumlah data: {len(df)}")

# --- Tahap 2: deteksi anomali pada data asli ---
df["zscore_asli"], rata2_asli, std_asli = hitung_zscore(df["suhu"])
df["anomali_asli"] = df["zscore_asli"].abs() > AMBANG_Z
jumlah_asli = int(df["anomali_asli"].sum())

# --- Tahap 3: moving average (jendela 5) lalu deteksi anomali ---
df["suhu_ma"] = df["suhu"].rolling(window=JUMLAH_DATA, min_periods=1).mean()
df["zscore_ma"], rata2_ma, std_ma = hitung_zscore(df["suhu_ma"])
df["anomali_ma"] = df["zscore_ma"].abs() > AMBANG_Z
jumlah_ma = int(df["anomali_ma"].sum())

# --- Tahap 4: tampilkan hasil dan perbandingan ---
print("\n=== Data asli ===")
print(f"Rata-rata  : {rata2_asli:.2f}")
print(f"Std deviasi: {std_asli:.2f}")
print(f"Minimum    : {df['suhu'].min():.2f}")
print(f"Maksimum   : {df['suhu'].max():.2f}")
print(f"Jumlah anomali: {jumlah_asli}")

print(f"\n=== Moving average (jendela {JUMLAH_DATA}) ===")
print(f"Rata-rata  : {rata2_ma:.2f}")
print(f"Std deviasi: {std_ma:.2f}")
print(f"Minimum    : {df['suhu_ma'].min():.2f}")
print(f"Maksimum   : {df['suhu_ma'].max():.2f}")
print(f"Jumlah anomali: {jumlah_ma}")

print("\n=== Perbandingan ===")
print(f"Anomali tanpa moving average : {jumlah_asli}")
print(f"Anomali dengan moving average: {jumlah_ma}")
print(f"Selisih                      : {jumlah_asli - jumlah_ma}")

# --- Tahap 5: visualisasi ---
fig, axes = plt.subplots(2, 1, figsize=(11, 7), sharex=True)

axes[0].plot(df.index, df["suhu"], marker="o", label="Suhu asli")
anom_asli = df[df["anomali_asli"]]
axes[0].scatter(
    anom_asli.index, anom_asli["suhu"], color="red", s=100,
    label="Anomali", zorder=3
)
axes[0].axhline(rata2_asli, color="gray", linestyle="--", label="Rata-rata")
axes[0].set_ylabel("Suhu (°C)")
axes[0].set_title(f"Data Asli - Anomali: {jumlah_asli}")
axes[0].legend()

axes[1].plot(df.index, df["suhu_ma"], marker="o", color="green", label="Moving average")
anom_ma = df[df["anomali_ma"]]
axes[1].scatter(
    anom_ma.index, anom_ma["suhu_ma"], color="red", s=100,
    label="Anomali", zorder=3
)
axes[1].axhline(rata2_ma, color="gray", linestyle="--", label="Rata-rata")
axes[1].set_xlabel("Urutan data")
axes[1].set_ylabel("Suhu (°C)")
axes[1].set_title(f"Moving Average (jendela {JUMLAH_DATA}) - Anomali: {jumlah_ma}")
axes[1].legend()

plt.tight_layout()
plt.savefig("grafik_suhu_modifikasi.png")
plt.show()
