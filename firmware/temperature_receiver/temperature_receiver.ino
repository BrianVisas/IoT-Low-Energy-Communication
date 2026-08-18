#include <LiquidCrystal.h>

namespace {
constexpr int kFanPin = 9;
constexpr int kHttpPort = 80;
constexpr unsigned long kReadIntervalMs = 15000;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

String ssid = "Simulator Wifi";
String password = "";
String host = "api.thingspeak.com";
String channelId = "1626114";

bool waitForResponse(const char* expected, unsigned long timeoutMs) {
  Serial.setTimeout(timeoutMs);
  return Serial.find(expected);
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

bool readSignedFieldValue(int& value) {
  if (!waitForResponse("\"field1\":", 3000)) return false;

  const unsigned long deadline = millis() + 2000;
  String number;
  bool started = false;

  while (millis() < deadline) {
    if (!Serial.available()) continue;

    const char c = static_cast<char>(Serial.read());
    if (!started && (c == '"' || c == ' ')) continue;

    if (c == '-' || isDigit(c)) {
      number += c;
      started = true;
      continue;
    }

    if (started) break;
  }

  if (number.length() == 0 || number == "-") return false;
  value = number.toInt();
  return true;
}

bool fetchTemperature(int& temperature) {
  if (!openTcpConnection()) return false;

  const String uri = "/channels/" + channelId + "/fields/1/last.json";
  const String request = "GET " + uri + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";

  Serial.print("AT+CIPSEND=");
  Serial.println(request.length());
  if (!waitForResponse(">", 1500)) return false;

  Serial.print(request);
  return readSignedFieldValue(temperature);
}

void applyFanCommand(int temperature) {
  const int boundedTemperature = constrain(temperature, -20, 100);
  const int pwm = map(boundedTemperature, -20, 100, 0, 255);
  analogWrite(kFanPin, pwm);
}

void showTemperature(int temperature) {
  lcd.setCursor(0, 0);
  lcd.print("Cloud temp      ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(temperature);
  lcd.print(" C");
}
}  // namespace

void setup() {
  pinMode(kFanPin, OUTPUT);
  analogWrite(kFanPin, 0);

  lcd.begin(16, 2);
  lcd.print("Connecting...");

  if (!connectWifi()) {
    lcd.clear();
    lcd.print("Network error");
  }
}

void loop() {
  int temperature = 0;
  if (fetchTemperature(temperature)) {
    applyFanCommand(temperature);
    showTemperature(temperature);
  } else {
    analogWrite(kFanPin, 0);
    lcd.setCursor(0, 0);
    lcd.print("Read error      ");
  }

  delay(kReadIntervalMs);
}
