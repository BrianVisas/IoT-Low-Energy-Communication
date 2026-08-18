// Gas sensor publisher for the original Tinkercad/ESP8266 experiment.
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int gas;
String ssid = "Simulator Wifi";
String password = "";
String host = "api.thingspeak.com";
const int httpPort = 80;

// Insert a valid ThingSpeak write API key before running the sketch.
// Never commit active credentials to source control.
String writeApiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";

int setupESP8266(void) {
  Serial.begin(115200);
  Serial.println("AT");
  delay(10);
  if (!Serial.find("OK")) return 1;

  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  delay(10);
  if (!Serial.find("OK")) return 2;

  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  delay(50);
  if (!Serial.find("OK")) return 3;

  return 0;
}

void publishGasLevel(void) {
  gas = map(analogRead(A0), 300, 750, 0, 100);

  String uri = "/update?api_key=" + writeApiKey + "&field1=";
  String httpPacket =
      "GET " + uri + String(gas) + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(gas);
  lcd.print(" %");

  Serial.print("AT+CIPSEND=");
  Serial.println(httpPacket.length());
  delay(10);

  Serial.print(httpPacket);
  delay(10);
  if (!Serial.find("SEND OK\r\n")) return;
}

void setup() {
  lcd.begin(16, 2);
  setupESP8266();
  lcd.print("Pollution");
}

void loop() {
  publishGasLevel();
  delay(15000);
}
