#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

/* Serial Baud Rate */
#define SERIAL_BAUD       115200
/* Delay paramter for connection. */
#define WIFI_DELAY        500
/* Max SSID octets. */
#define MAX_SSID_LEN      32
/* Wait this much until device gets IP. */
#define MAX_CONNECT_TIME  30000
/* SSID that to be stored to connect. */
char ssid[MAX_SSID_LEN] = "";
/* Scan available networks and sort them in order to their signal strength. */
// #define LED_PIN 5     //pin D1
// #define BUZZER_PIN 4  //pin D4
const int pinD3 = D3;    //pin D3
const int LED_PIN= D7;  //
const int BUZZER_PIN= D6;// 
 
// variabel untuk host server
String firstThreeOctets;

void scanAndSort() {
  memset(ssid, 0, MAX_SSID_LEN);
  int n = WiFi.scanNetworks();
  Serial.println("Scan complete!");
  if (n == 0) {
    Serial.println("No networks available.");
  } else {
    Serial.print(n);
    Serial.println(" networks discovered.");
    int indices[n];
    for (int i = 0; i < n; i++) {
      indices[i] = i;
    }
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
          std::swap(indices[i], indices[j]);
        }
      }
    }
    for (int i = 0; i < n; ++i) {
      Serial.println("The strongest open network is:");
      Serial.print(WiFi.SSID(indices[i]));
      Serial.print(" ");
      Serial.print(WiFi.RSSI(indices[i]));
      Serial.print(" ");
      Serial.print(WiFi.encryptionType(indices[i]));
      Serial.println();
      if(WiFi.encryptionType(indices[i]) == ENC_TYPE_NONE) {
        memset(ssid, 0, MAX_SSID_LEN);
        strncpy(ssid, WiFi.SSID(indices[i]).c_str(), MAX_SSID_LEN);
        break;
      }
    }
  }
}
void setup() {
  Serial.begin(SERIAL_BAUD);
  boolean a;
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
   pinMode(pinD3,INPUT);
  lcd.begin();  // Inisialisasi LCD 16x2
  Serial.println("Scanning for open networks...");
  lcd.print("Mencari Jaringan");
  lcd.setCursor(0, 1);
  lcd.print("Mohon menunggu");
  delay(5000);  // Tahan tampilan selama 2 detik
  lcd.clear();
   if(WiFi.status() != WL_CONNECTED) {
    /* Clear previous modes. */
    WiFi.softAPdisconnect();
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    delay(WIFI_DELAY);
    /* Scan for networks to find open guy. */
    scanAndSort();
    delay(WIFI_DELAY);
    /* Global ssid param need to be filled to connect. */
    if(strlen(ssid) > 0) {
      Serial.print("Connecting to ");
      Serial.println(ssid);
      /* No pass for WiFi. We are looking for non-encrypteds. */
      WiFi.begin(ssid);
      unsigned short try_cnt = 0;
      /* Wait until WiFi connection but do not exceed MAX_CONNECT_TIME */
      while (WiFi.status() != WL_CONNECTED && try_cnt < MAX_CONNECT_TIME / WIFI_DELAY) {
        delay(WIFI_DELAY);
        Serial.print(".");
        try_cnt++;
      }
      if(WiFi.status() == WL_CONNECTED) {
        Serial.println("");
         delay(2000);
        Serial.println("Connection Successful!");
        Serial.println("Your device IP address is ");
        Serial.println(WiFi.localIP().toString());
        digitalWrite(LED_PIN, HIGH);
        lcd.print("Wifi Terkoneksi");
        lcd.setCursor(0, 1);
        lcd.print("Koneksi Berhasil");
        delay(2000);  // Tahan tampilan selama 2 detik
        lcd.clear();
      }
    }
   }
}

const char* host= "lozengy-principals.000webhostapp.com";
void loop() {
  lcd.print("Wifi Terkoneksi");
  lcd.setCursor(0, 1);
  Serial.println(WiFi.localIP());
  lcd.print(WiFi.localIP());
  int a=digitalRead(pinD3);
  Serial.println(a);
  if (a==0){
    Serial.println(a);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(20000);
  }
  digitalWrite(BUZZER_PIN, LOW);
  
  // put your main code here, to run repeatedly:
  // baca status koneksi ke server
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Koneksi ke server bermasalah");
    return;
  }

  // apabila berhasil konek ke server / database
  String Link;
  HTTPClient http;
  Link = "http://" + String(host) +"/bacadata.php";
  // eksekusi link
  http.begin(Link);
  http.GET();
  
  //baca feedback
  String hasil = http.getString();
  Serial.println(hasil);
  http.end();
  if(hasil=="Bunyikan"){
    digitalWrite(BUZZER_PIN, HIGH);
    //Kembalikan status jam ke 0 agar tidak bunyi bel terus menerus
    Link = "http://" + String(host) +"/ubahstatus.php";
    http.begin(Link);
    http.GET();

    // jeda waktu
    delay(10000);
  }
   digitalWrite(BUZZER_PIN, LOW);
  
  delay(1000);
}

