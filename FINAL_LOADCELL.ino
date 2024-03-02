  // Library Load Cell
  #include <HX711.h>
  const int LOADCELL_DOUT_PIN = D5; // Pin data HX711 terhubung ke pin 3 pada Arduino
  const int LOADCELL_SCK_PIN = D6; // Pin clock HX711 terhubung ke pin 2 pada Arduino
  HX711 scale;

  //Library ESP8266
  #include <ESP8266WiFi.h>
  char ssid[]="ned";
  char pass[]="nadifa2001";
  const char *host = "172.20.10.3";
  
  int value = 0;
    
void setup() {
  Serial.begin(9600);

  // Koneksi ke Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Konfigurasi Load Cell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(203); // Set faktor kalibrasi ke 203
  scale.tare(); // Set tare ke 0
}

void loop() {
  int weight = scale.get_units();

  if (weight > 0) {
    upload();
    
  } else {
    Serial.println("Weight : 0 g");
  }
  
  delay(1000);
}

void upload() {
// Proses Pengiriman
  ++value;
 
// Membaca Sensor Analog
  int weight = scale.get_units(); // Mendapatkan berat dalam satuan yang telah dikalibrasi
  
  Serial.println("Weight : " + String(weight) + " g");
 
// Menghubungkan ke server (host) dengan menggunakan objek WiFiClient dan memeriksa koneksi
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    Serial.println();
    return;
  }
 
// Membentuk URL yang akan dikirimkan ke server, dengan menggabungkan nilai suhu dan kelembaban dalam URL yang dituju
  String data_berat = "/writedata/write_data.php?berat=";
  data_berat += weight;
  
 
// Mengirimkan request HTTP ke server menggunakan client.print() dan menunggu hingga data tersedia dengan menggunakan while loop
  client.print(String("GET /writedata/write_data.php?berat=") + weight +  " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 1000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  Serial.println("Done Uploading");
  Serial.println();
}
