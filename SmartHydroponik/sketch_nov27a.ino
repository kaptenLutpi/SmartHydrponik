// #define BLYNK_TEMPLATE_ID "TMPL64gnSIQvb"
// #define BLYNK_TEMPLATE_NAME "SmartHydroponik"
// #define BLYNK_AUTH_TOKEN "EZAh4aUu78AkLWcFbwHtOJHSdnmbCpFx"
// #define BLYNK_PRINT Serial 

#define BLYNK_TEMPLATE_ID "TMPL65VJicUFk"
#define BLYNK_TEMPLATE_NAME "SmartHydroponik"
#define BLYNK_AUTH_TOKEN "T5gbszYLyyRgyzYGaX9YMrFasXRpB7y9"
#define BLYNK_PRINT Serial 

#include <Blynk.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h> // Library Bly


//Konektivitas Ke Wfi
char auth[] = BLYNK_AUTH_TOKEN; // Ganti dengan token proyek Blynk Anda
const char* ssid = "jerome valeska!!";     // Ganti dengan nama jaringan WiFi Anda
const char*password = "Sendy233"; // Ganti dengan kata sandi WiFi Anda
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Alamat I2C modul LCD


//Deklarasi pin sensor tds dan ultrasonik agar bisa digunakan
const int sensorPin = A0;  // Pin sensor TDS terhubung ke pin Analog A0
const int trigPin = D5; // Pin Trigger sensor ultrasonik terhubung ke D1 (GPIO5) pada NodeMCU
const int echoPin = D6; // Pin Echo sensor ultrasonik terhubung ke D2 (GPIO4) pada NodeMCU
// const int relayPin = D3; // Pin relay terhubung ke D7 (GPIO13) pada NodeMCU

//mengatur waktu pada blynk
BlynkTimer timer; //deklarasi dari objek timer yang berasal dari kelas BlynkTimer untuk membuat dan 
bool Relay = 0; //input awal Relay 0/ Off
bool isAutoMode = true; // Mode awal: otomatis


#define pumpTds D3 //deklarasi pin pump pada esp
#define pumpTds1 D4 // deklarasi pin relay kedua pada esp



void sendSensorData() {  
  int sensorValue = analogRead(sensorPin); //untuk membaca nilai analog dari pin yang ditentukan oleh Sensor pin
  float voltage = sensorValue * (3.3 / 1024.0);//mengkonversi nilai pada sensorValue ke dalam tegangan 
  float tdsValue = voltage * 420;//mengonversi tengangan yang terukur menjadi nilai pada sensor TDS

  Blynk.virtualWrite(V0, tdsValue); // Menulis nilai ppm ke pin V5 di Blynk
  
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;

  

  // if (tdsValue > 30) { // Ubah nilai batas sesuai kebutuhan Anda
  //   digitalWrite(pumpTds, LOW); // Aktifkan relay jika nilai TDS melebihi batas
  // } else {
  //   digitalWrite(pumpTds, HIGH); // Matikan relay jika nilai TDS dalam batas normal
  // }

  Blynk.virtualWrite(V3, distance); // Menulis nilai sensor ultrasonik ke pin V6 di Blynk

  // if(distance > 50){
  //     Serial.println("Distance above 50!"); // Debug line to check if the condition is being met
  //   Blynk.logEvent("Air_Tendon_Menipis");
  // } 
  
}


void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, password, "blynk.cloud", 80); // Inisialisasi koneksi Blynk dengan template ID dan name
  
  timer.setInterval(1000L, sendSensorData);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(pumpTds, OUTPUT);
  digitalWrite(pumpTds, HIGH); // Matikan relay saat awal program dijalankan
  delay(100);   
  lcd.init();
  lcd.backlight();

  // Menghubungkan ke jaringan WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  lcd.setCursor(0, 1);
  lcd.print("IP: ");
  lcd.print(WiFi.localIP());
  lcd.clear();
 
  
}
BLYNK_WRITE(V1) { // Fungsi ini dipanggil setiap kali tombol switch diubah di aplikasi Blynk
   int Relay = param.asInt(); // Membaca nilai dari tombol switch di Blynk
  if (Relay == 1) {
    digitalWrite(pumpTds, LOW); // Aktifkan relay 1
    digitalWrite(pumpTds1, LOW); // Aktifkan relay 2
  } else {
    digitalWrite(pumpTds, HIGH); // Matikan relay 1
    digitalWrite(pumpTds1, HIGH); // Matikan relay 2
  }
  
}
BLYNK_WRITE(V2) { // Toggle button untuk mode manual/otomatis
  isAutoMode = !isAutoMode; // Toggle antara mode otomatis dan manual
  if (isAutoMode) {
    // lcd.setCursor(0, 1);
    // lcd.print("Manual Mode ");
  } else {
    // lcd.setCursor(0, 1);
    // lcd.print("Manual Mode");
  }
}

 
void autoPumpControl() {
  int sensorValue = analogRead(sensorPin);
  float voltage = sensorValue * (3.3 / 1024.0);
  float tdsValue = voltage * 420;

  if (isAutoMode) {
    if (tdsValue >= 100) {
      digitalWrite(pumpTds, LOW); // Aktifkan relay jika nilai TDS melebihi batas
      // lcd.setCursor(0, 1);
      // lcd.print("Auto Mode ");
    } else {
      digitalWrite(pumpTds, HIGH); // Matikan relay jika nilai TDS dalam batas normal
      // lcd.setCursor(0, 1);
      // lcd.print("Auto Mode");
    }
  }
  Blynk.virtualWrite(V0, tdsValue); // Menulis nilai ppm ke pin V5 di Blynk
}


void loop() {
  int sensorValue = analogRead(sensorPin);
  // Ubah nilai analog menjadi nilai TDS
  float voltage = sensorValue * (3.3 / 1024.0); // Konversi nilai ADC ke tegangan (NodeMCU menggunakan tegangan 3.3V)
  float tdsValue = voltage * 420; // Konversi tegangan ke nilai TDS (dalam ppm)

  lcd.setCursor(0, 0);
  lcd.print("TDS: ");
  lcd.print(tdsValue);
  lcd.print(" ppm    ");

  Serial.print("TDS Value: ");
  Serial.print(tdsValue);
  Serial.println(" ppm");

  long duration, distance;
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  
  // Hitung jarak berdasarkan durasi gelombang ultrasonik
  distance = (duration * 0.0343) / 2; // Faktor konversi untuk mengubah durasi menjadi jarak (dalam cm)
  
  lcd.setCursor(0, 1);
  lcd.print("Height: ");
  lcd.print(distance);
  lcd.print(" cm    ");

  Serial.print("Height: ");
  Serial.print(distance);
  Serial.println(" cm");
  

  delay(1000); // Tunggu satu detik sebelum membaca ulang sensor
  Blynk.run(); // Menjalankan Blynk
  timer.run();
  autoPumpControl();// memanggil class untuk di jalankan pada loop

}