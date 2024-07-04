#define BLYNK_TEMPLATE_NAME "soilmeter"
#define BLYNK_AUTH_TOKEN "authtoken"
#define BLYNK_TEMPLATE_ID "templateid"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const int moisturePin = A0;
int moisture = 0;
int moistureDry = 760;
int moistureWet = 360; 

const char* ssid = "testssid";
const char* password = "testpassword";

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP!");

  pinMode(moisturePin, INPUT);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  Serial.println("Blynk connected");
}

void loop() {
  Blynk.run();

  // Read moisture level from analog pin
  moisture = analogRead(moisturePin);
  Serial.print("Moisture Level: ");
  Serial.println(moisture);
  moisture = map(moisture, moistureDry, moistureWet, 0, 100);  // Map the moisture reading to a 0-100 range
  Serial.print("Moisture Level (0-100): ");
  Serial.println(moisture);
  // Print moisture level to Serial Monitor

  // Send moisture level to Blynk virtual pin V0
  Blynk.virtualWrite(V0, moisture);

  delay(100000); // Optional delay to avoid flooding Blynk with updates
}
