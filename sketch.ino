#define BLYNK_TEMPLATE_ID "TMPL3IgWAi-cP"
#define BLYNK_TEMPLATE_NAME "Smart irrigation System"
#define BLYNK_AUTH_TOKEN "2_j9NoYaNpe9coBoN9hSX-KDEdpTZokq"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define POTENTIOMETER_PIN 34
#define DHT_PIN 15
#define RELAY_PIN 4

#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);
BlynkTimer timer;

const int DRY_VALUE = 4095;
const int WET_VALUE = 0;

int moisturePercent = 0;
bool manualPumpState = false;

BLYNK_WRITE(V2) {
int pumpSwitch = param.asInt();
if (pumpSwitch == 1) {
digitalWrite(RELAY_PIN, HIGH);
manualPumpState = true;
} else {
digitalWrite(RELAY_PIN, LOW);
manualPumpState = false;
}
}

void sendSensorData() {
float temperature = dht.readTemperature();
if (isnan(temperature)) {
Serial.println("Failed to read from DHT sensor!");
return;
}

int moistureValue = analogRead(POTENTIOMETER_PIN);
moisturePercent = map(moistureValue, DRY_VALUE, WET_VALUE, 0, 100);

if (moisturePercent > 100) moisturePercent = 100;
if (moisturePercent < 0) moisturePercent = 0;

Serial.print("Simulated Moisture: ");
Serial.print(moisturePercent);
Serial.print("% | Temperature: ");
Serial.print(temperature);
Serial.println(" °C");

Blynk.virtualWrite(V0, temperature);
Blynk.virtualWrite(V1, moisturePercent);

if (!manualPumpState) {
if (moisturePercent < 30) {
digitalWrite(RELAY_PIN, HIGH);
Serial.println("Soil is dry! Pump ON.");
} else {
digitalWrite(RELAY_PIN, LOW);
Serial.println("Soil is moist. Pump OFF.");
}
} else {
Serial.println("Pump is in manual override mode.");
}
}

void setup() {
Serial.begin(115200);

dht.begin();
pinMode(RELAY_PIN, OUTPUT);
digitalWrite(RELAY_PIN, LOW);
Serial.println("Hello");
Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

timer.setInterval(2000L, sendSensorData);
}

void loop() {
Blynk.run();
timer.run();
}
