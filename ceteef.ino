// Mengimpor pustaka untuk LCD dengan komunikasi I2C
#include <LiquidCrystal_I2C.h>
// Mengimpor pustaka untuk mengontrol motor servo
#include <Servo.h>

// Menginisialisasi objek LCD I2C dengan alamat 0x27 atau 0x3F
// Parameter: (alamat I2C, jumlah kolom, jumlah baris)
LiquidCrystal_I2C lcd(32, 16, 2);  // Ganti ke 0x3F jika LCD tidak tampil

// Mendefinisikan pin-pin yang digunakan dalam rangkaian
const int buzzerPin = 10;     // Pin untuk buzzer
const int ledRed = 12;        // Pin untuk LED merah
const int ledGreen = 11;      // Pin untuk LED hijau
const int soilSensorPin = A0; // Pin analog untuk sensor kelembapan tanah
const int servoPin = 9;       // Pin untuk motor servo

// Membuat objek servo untuk mengontrol motor servo
Servo moistureServo;

void setup() {
  // Mengatur mode pin sebagai OUTPUT
  pinMode(buzzerPin, OUTPUT);  // Pin buzzer sebagai output
  pinMode(ledRed, OUTPUT);     // Pin LED merah sebagai output
  pinMode(ledGreen, OUTPUT);   // Pin LED hijau sebagai output
  
  // Inisialisasi servo
  moistureServo.attach(servoPin);       // Menghubungkan servo ke pin yang ditentukan
  moistureServo.write(0);                // Mengatur posisi awal servo ke 0 derajat
  
  // Memulai komunikasi serial dengan kecepatan 9600 bps
  Serial.begin(9600);

  // Inisialisasi LCD
  lcd.init();                  // Memulai LCD
  lcd.backlight();             // Menyalakan lampu latar belakang LCD

  // Menampilkan tampilan awal pada LCD
  lcd.setCursor(0, 0);         // Mengatur kursor ke posisi (kolom 0, baris 0)
  lcd.print("Sistem Penyiram"); // Menampilkan teks pada baris pertama
  lcd.setCursor(0, 1);         // Mengatur kursor ke posisi (kolom 0, baris 1)
  lcd.print("Tanaman Otomatis"); // Menampilkan teks pada baris kedua
  delay(2500);                 // Menunggu selama 2,5 detik
  
  // Menghapus tampilan LCD
  lcd.clear();
  
  // Menampilkan label-label pada LCD
  lcd.setCursor(0, 0);         // Mengatur kursor ke posisi (kolom 0, baris 0)
  lcd.print("Kelembapan =");    // Menampilkan label untuk kelembapan
  lcd.setCursor(0, 1);         // Mengatur kursor ke posisi (kolom 0, baris 1)
  lcd.print("Keran = ");        // Menampilkan label untuk status keran
}

void loop() {
  // Membaca nilai analog dari sensor kelembapan tanah
  int sensorValue = analogRead(soilSensorPin);
  
  // Mengkonversi nilai sensor (0-876) menjadi persentase kelembapan (0-99%)
  // 0 = kering, 99 = basah
  int moisture = map(sensorValue, 0, 876, 0, 99); 

  // Menampilkan nilai kelembapan pada Serial Monitor
  Serial.print("Kelembapan: ");
  Serial.println(moisture);

  // Menampilkan nilai kelembapan pada LCD
  lcd.setCursor(13, 0);        // Mengatur kursor ke posisi (kolom 13, baris 0)
  lcd.print(moisture);         // Menampilkan nilai kelembapan
  lcd.print("%");              // Menampilkan simbol persen

  // Variabel untuk menyimpan posisi servo
  int servoPos;
  
  // Menghapus status keran sebelumnya pada LCD
  lcd.setCursor(8, 1);
  lcd.print("        ");  // Memberikan spasi yang cukup untuk menghapus nilai sebelumnya
  lcd.setCursor(8, 1);
  
  // Menentukan posisi servo dan tampilan status keran berdasarkan tingkat kelembapan
  if (moisture < 30) {
    // Kelembapan di bawah 30%: servo terbuka penuh (0 derajat)
    servoPos = 0;
    digitalWrite(ledGreen, HIGH);    // Menghidupkan LED hijau
    digitalWrite(ledRed, LOW);       // Mematikan LED merah
    playSound();                     // Memanggil fungsi untuk membunyikan buzzer
    
    // Menampilkan status keran "BUKA"
    lcd.print("BUKA");
  } else if (moisture > 80) {
    // Kelembapan di atas 80%: servo tertutup penuh (90 derajat)
    servoPos = 90;
    digitalWrite(ledGreen, LOW);     // Mematikan LED hijau
    digitalWrite(ledRed, HIGH);      // Menghidupkan LED merah
    
    // Menampilkan status keran "TUTUP"
    lcd.print("TUTUP");
  } else {
    // Kelembapan antara 30% dan 80%: servo bergerak secara proporsional
    // Map nilai kelembapan 30-80 ke sudut servo 0-90
    servoPos = map(moisture, 30, 80, 0, 90);
    
    // Menentukan status LED berdasarkan posisi servo
    if (servoPos < 45) {
      digitalWrite(ledGreen, HIGH);  // Menghidupkan LED hijau
      digitalWrite(ledRed, LOW);     // Mematikan LED merah
    } else {
      digitalWrite(ledGreen, LOW);   // Mematikan LED hijau
      digitalWrite(ledRed, HIGH);    // Menghidupkan LED merah
    }
    
    // Menghitung persentase bukaan keran (100% saat moisture 30%, 0% saat moisture 80%)
    int openPercentage = map(moisture, 30, 80, 100, 0);
    
    // Menampilkan persentase bukaan keran
    lcd.print(openPercentage);
    lcd.print("%");
  }
  
  // Menggerakkan servo ke posisi yang ditentukan
  moistureServo.write(servoPos);
  
  // Memberikan jeda kecil untuk stabilitas pergerakan servo
  delay(200);
}

// Fungsi untuk membunyikan buzzer
void playSound() {
  tone(buzzerPin, 1000, 100);  // Membunyikan buzzer dengan frekuensi 1000Hz selama 100ms
  delay(1000);                 // Menunggu selama 1 detik
  noTone(buzzerPin);           // Mematikan buzzer
}