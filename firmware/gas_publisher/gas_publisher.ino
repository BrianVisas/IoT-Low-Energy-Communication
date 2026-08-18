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
  Serial.setTimeout(timeoutMs);
  return Serial.find(expected);
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

int readGasPercentage() {
  const int raw = analogRead(kSensorPin);
  return constrain(map(raw, 300, 750, 0, 100), 0, 100);
}

void showGasLevel(int percentage) {
  lcd.setCursor(0, 0);
  lcd.print("Pollution       ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(percentage);
  lcd.print(" %");
}

bool publishGasLevel(int percentage) {
  if (!openTcpConnection()) return false;

  const String uri = "/update?api_key=" + writeApiKey + "&field1=" + String(percentage);
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
  const int gasPercentage = readGasPercentage();
  showGasLevel(gasPercentage);
  publishGasLevel(gasPercentage);
  delay(kPublishIntervalMs);
}
