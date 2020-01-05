#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

String readString;
const char* ssid = "YOUR SSID";
const char* password = "YOUR PASSWORD";

// Control variables
const int defaultDelay = 1000; // 1 Second
const int readDelay = 1800000; // 30 minutes
const int baudrateSerial = 74880;

// Request variables
const char* host = "script.google.com";
const int httpsPort = 443;
const int timeout = 15000; // 15 Seconds

const char* fingerprint = "02 68 AC 26 2F DE 4E E4 AF 9A 51 4B 6F 15 03 D0 C7 65 60 12";
String GAS_ID = "YOUR GOOGLE SCRIPT ID";  // Replace by your GAS service id

WiFiClientSecure client;

void setup() {
  setupSerial();
  setupWIFI();
}

void setupSerial() {
   Serial.begin(baudrateSerial);
}

void setupWIFI() {
  Serial.println("[Setup WIFI]");
  
  delay(defaultDelay);
  WiFi.mode(WIFI_OFF); //Prevents reconnection issue (taking too long to connect)
  delay(defaultDelay);

  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.print("\n");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.print("\n");
  
  sendData(1); //This function uploads data to Google Sheets
  
  delay(readDelay);
}

void sendData(int temperature) {
  Serial.println("[Send Data]");
  
  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);
  client.setTimeout(timeout);
  delay(defaultDelay);
  
  Serial.print("connecting to ");
  Serial.println(host);
  
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }
  
  String string_temp = String(temperature, DEC);
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temp;
  
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  //String response = client.readString();
  //Serial.println(response);

  Serial.println("closing connection");
}
