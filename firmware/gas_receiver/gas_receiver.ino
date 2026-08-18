#include <LiquidCrystal.h>
#include <Servo.h>

namespace {
constexpr int kServoPin = 9;
constexpr int kHttpPort = 80;
constexpr unsigned long kReadIntervalMs = 15000;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo actuatorServo;

String ssid = "Simulator Wifi";
String password = "";
String host = "api.thingspeak.com";
String channelId = "1626248";

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

bool connectWifi() {
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

bool readUnsignedFieldValue(int& value) {
  if (!waitForResponse("\"field1\":", 3000)) return false;

  const unsigned long deadline = millis() + 2000;
  String number;
  bool started = false;

  while (millis() < deadline) {
    if (!Serial.available()) continue;

    const char c = static_cast<char>(Serial.read());
    if (!started && (c == '"' || c == ' ')) continue;

    if (isDigit(c)) {
      number += c;
      started = true;
      continue;
    }

    if (started) break;
  }

  if (number.length() == 0) return false;
  value = number.toInt();
  return true;
}

bool fetchGasLevel(int& percentage) {
  if (!openTcpConnection()) return false;

  const String uri = "/channels/" + channelId + "/fields/1/last.json";
  const String request = "GET " + uri + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";

  Serial.print("AT+CIPSEND=");
  Serial.println(request.length());
  if (!waitForResponse(">", 1500)) return false;

  Serial.print(request);
  return readUnsignedFieldValue(percentage);
}

void applyServoCommand(int percentage) {
  const int boundedPercentage = constrain(percentage, 0, 100);
  const int angle = map(boundedPercentage, 0, 100, 0, 180);
  actuatorServo.write(angle);
}

void showGasLevel(int percentage) {
  lcd.setCursor(0, 0);
  lcd.print("Cloud pollution ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(percentage);
  lcd.print(" %");
}
}  // namespace

void setup() {
  actuatorServo.attach(kServoPin, 500, 2500);
  actuatorServo.write(0);

  lcd.begin(16, 2);
  lcd.print("Connecting...");

  if (!connectWifi()) {
    lcd.clear();
    lcd.print("Network error");
  }
}

void loop() {
  int gasPercentage = 0;
  if (fetchGasLevel(gasPercentage)) {
    gasPercentage = constrain(gasPercentage, 0, 100);
    applyServoCommand(gasPercentage);
    showGasLevel(gasPercentage);
  } else {
    actuatorServo.write(0);
    lcd.setCursor(0, 0);
    lcd.print("Read error      ");
  }

  delay(kReadIntervalMs);
}
