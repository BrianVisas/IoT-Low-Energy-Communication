#include <LiquidCrystal.h>

namespace {
constexpr int kSensorPin = A0;
constexpr unsigned long kPublishIntervalMs = 15000;
constexpr int kHttpPort = 80;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

String ssid = "Simulator Wifi";
String password = "";
String host = "api.thingspeak.com";
String writeApiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";

bool waitForResponse(const char* expected, unsigned long timeoutMs) {
  if (expected == nullptr || expected[0] == '\0') return false;

  size_t matched = 0;
  const unsigned long start = millis();

  while (millis() - start < timeoutMs) {
    if (!Serial.available()) continue;

    const char c = static_cast<char>(Serial.read());
    if (c == expected[matched]) {
      ++matched;
      if (expected[matched] == '\0') return true;
    } else {
      matched = (c == expected[0]) ? 1 : 0;
    }
  }

  return false;
}

bool connectEsp8266() {
  Serial.begin(115200);

  Serial.println("AT");
  if (!waitForResponse("OK", 1000)) return false;

  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  return waitForResponse("OK", 5000);
}

bool openTcpConnection() {
  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + String(kHttpPort));
  return waitForResponse("OK", 3000);
}

int readTemperatureCelsius() {
  return map(analogRead(kSensorPin), 20, 358, -20, 100);
}

void showTemperature(int temperature) {
  lcd.setCursor(0, 0);
  lcd.print("Temperature     ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(temperature);
  lcd.print(" C");
}

bool publishTemperature(int temperature) {
  if (!openTcpConnection()) return false;

  const String uri = "/update?api_key=" + writeApiKey + "&field1=" + String(temperature);
  const String request = "GET " + uri + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";

  Serial.print("AT+CIPSEND=");
  Serial.println(request.length());
  if (!waitForResponse(">", 1500)) return false;

  Serial.print(request);
  return waitForResponse("SEND OK", 2000);
}
}  // namespace

void setup() {
  lcd.begin(16, 2);
  lcd.print("Connecting...");

  if (!connectEsp8266()) {
    lcd.clear();
    lcd.print("Network error");
  }
}

void loop() {
  const int temperature = readTemperatureCelsius();
  showTemperature(temperature);
  publishTemperature(temperature);
  delay(kPublishIntervalMs);
}
