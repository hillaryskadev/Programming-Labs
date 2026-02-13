#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 20  // jumlah data (bisa diganti, misal 20, 100, 288)

// Rentang data normal dan anomali
#define NORMAL_MIN 100
#define NORMAL_MAX 200
#define ANOMALY_LOW_MIN 0
#define ANOMALY_LOW_MAX 20
#define ANOMALY_HIGH_MIN 500
#define ANOMALY_HIGH_MAX 800

// ==== Deklarasi fungsi ====
void generate_data(int data[], int n, int *num_anomalies_generated);
double mean_all(int data[], int n);
double median_of_array(int data[], int n);
int is_anomaly(int value);
void compute_minmax_correct(int data[], int n, int *min, int *max);
void filter_anomalies_median_neighborhood(int data[], int n);
void print_first_elements_with_marks(int data[], int n);
void print_histogram(int data[], int n);
void print_stats(int data[], int n);

// ==== Fungsi utama ====
int main() {
    srand(time(NULL));  // seed random
    int data[N];
    int anomalies_generated = 0;

    // 1️⃣ Generate data
    generate_data(data, N, &anomalies_generated);
    printf("\nJumlah anomali yang dihasilkan: %d\n", anomalies_generated);

    // 2️⃣ Statistik sebelum filter
    printf("\n--- Statistik SEBELUM filter ---\n");
    print_stats(data, N);

    // 3️⃣ Visualisasi sebelum filter
    print_first_elements_with_marks(data, N);
    printf("\nHistogram distribusi data SEBELUM:\n");
    print_histogram(data, N);

    // 4️⃣ Filtering (perbaiki anomali)
    filter_anomalies_median_neighborhood(data, N);

    // 5️⃣ Statistik sesudah filter
    printf("\n--- Statistik SESUDAH filter ---\n");
    print_stats(data, N);

    // 6️⃣ Visualisasi sesudah filter
    print_first_elements_with_marks(data, N);
    printf("\nHistogram distribusi data SESUDAH:\n");
    print_histogram(data, N);

    return 0;
}

// ==== Fungsi pembuat data ====
void generate_data(int data[], int n, int *num_anomalies_generated) {
    int num_anom = 0;
    for (int i = 0; i < n; i++) {
        int chance = rand() % 100;
        if (chance < 5) {  // 5% anomali
            num_anom++;
            if (rand() % 2)
                data[i] = ANOMALY_LOW_MIN + rand() % (ANOMALY_LOW_MAX - ANOMALY_LOW_MIN + 1);
            else
                data[i] = ANOMALY_HIGH_MIN + rand() % (ANOMALY_HIGH_MAX - ANOMALY_HIGH_MIN + 1);
        } else {
            data[i] = NORMAL_MIN + rand() % (NORMAL_MAX - NORMAL_MIN + 1);
        }
    }
    *num_anomalies_generated = num_anom;  // simpan jumlah anomali ke variabel utama
}

// ==== Hitung rata-rata ====
double mean_all(int data[], int n) {
    long sum = 0;
    for (int i = 0; i < n; i++) sum += data[i];
    return (double)sum / n;
}

// ==== Hitung median ====
double median_of_array(int data[], int n) {
    int temp[N];
    for (int i = 0; i < n; i++) temp[i] = data[i];

    // bubble sort sederhana
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (temp[j] > temp[j + 1]) {
                int t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }

    if (n % 2 == 0)
        return (temp[n / 2 - 1] + temp[n / 2]) / 2.0;
    else
        return temp[n / 2];
}

// ==== Cek apakah nilai anomali ====
int is_anomaly(int value) {
    return (value < NORMAL_MIN || value > NORMAL_MAX);
}

// ==== Cari min dan max yang normal ====
void compute_minmax_correct(int data[], int n, int *min, int *max) {
    *min = 999999;
    *max = -999999;
    for (int i = 0; i < n; i++) {
        if (!is_anomaly(data[i])) {
            if (data[i] < *min) *min = data[i];
            if (data[i] > *max) *max = data[i];
        }
    }
}

// ==== Filter anomali dengan median tetangga ====
void filter_anomalies_median_neighborhood(int data[], int n) {
    for (int i = 0; i < n; i++) {
        if (is_anomaly(data[i])) {
            int neighbors[5];
            int count = 0;

            // ambil maksimal 2 tetangga kiri dan kanan
            for (int j = i - 2; j <= i + 2; j++) {
                if (j >= 0 && j < n && !is_anomaly(data[j])) {
                    neighbors[count++] = data[j];
                }
            }

            if (count > 0) {
                // sort untuk ambil median
                for (int a = 0; a < count - 1; a++)
                    for (int b = 0; b < count - a - 1; b++)
                        if (neighbors[b] > neighbors[b + 1]) {
                            int t = neighbors[b];
                            neighbors[b] = neighbors[b + 1];
                            neighbors[b + 1] = t;
                        }

                // ganti nilai anomali dengan median tetangganya
                data[i] = neighbors[count / 2];
            }
        }
    }
}

// ==== Tampilkan 50 elemen pertama (atau semua jika N < 50) ====
void print_first_elements_with_marks(int data[], int n) {
    int limit = n < 50 ? n : 50;  // batas tampilan
    printf("\n=== %d elemen pertama (anomali = '!') ===\n", limit);

    for (int i = 0; i < limit; i++) {
        printf("%2d:%4d", i, data[i]); // format rapi
        if (is_anomaly(data[i]))
            printf(" !");
        else
            printf("  ");

        if ((i + 1) % 10 == 0)
            printf("\n");
        else
            printf("  ");
    }
    printf("\n");
}

// ==== Buat histogram sederhana ====
void print_histogram(int data[], int n) {
    int bins[11] = {0};  // rentang 100–200 dibagi jadi 10 kelompok

    for (int i = 0; i < n; i++) {
        if (!is_anomaly(data[i])) {
            int index = (data[i] - NORMAL_MIN) / 10;
            if (index < 0) index = 0;
            if (index > 10) index = 10;
            bins[index]++;
        }
    }

    for (int i = 0; i <= 10; i++) {
        int start = NORMAL_MIN + i * 10;
        int end = start + 9;
        printf("%3d-%3d | %3d | ", start, end, bins[i]);
        for (int j = 0; j < bins[i]; j++)
            printf("*");
        printf("\n");
    }
}

// ==== Cetak statistik (mean, median, min, max) ====
void print_stats(int data[], int n) {
    double mean = mean_all(data, n);
    double median = median_of_array(data, n);
    int min, max;
    compute_minmax_correct(data, n, &min, &max);

    printf("Rata-rata : %.2f\n", mean);
    printf("Median    : %.2f\n", median);
    printf("Min valid : %d\n", min);
    printf("Max valid : %d\n", max);
}
