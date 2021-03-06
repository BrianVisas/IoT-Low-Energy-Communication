// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Variables globales
int Motorspeed;
const int Led_Error = 6;	// Status LEDs
const int Led_Ok = 7;
const int  fan = 5;
byte Actuator; 				// Signal for the actuator
int CycleAct;			// Read cycle counter to change the actuator
// Configuration parameters for WIFI connection
String ssid     = "Simulator Wifi";  // Name of the WIFI network to which it connects (SSID)
String password = ""; 
String host     = "api.thingspeak.com";
const int httpPort   = 80;

String urlRead = "/channels/1626114/fields/1/last.json";
//Replace 1626114 with the channel's PUBLIC read key in ThingSpeak

// Hardware configuration function
void setup() {
   // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
	// Configure the operation indicator LEDs
	pinMode(Led_Error,OUTPUT);
	pinMode(Led_Ok,OUTPUT);
	digitalWrite(Led_Ok, HIGH);
	digitalWrite(Led_Error, HIGH);
  	delay(2000); 
	digitalWrite(Led_Ok, LOW);
	digitalWrite(Led_Error, LOW);
  	setupESP8266();		//Configure the WIFI connection module ESP8266
  	CycleAct = 0;
  lcd.print("Temperature: !");
  delay(10);
}

// Infinite control cycle
void loop() {
	if (digitalRead(Led_Ok)) {
		digitalWrite(Led_Ok, LOW);
	}
	else {
		digitalWrite(Led_Ok, HIGH);
	}
	// Delay for time measurement
	delay(500);
  	
    	UpdateActuator();
     
}

// Connection subroutine of the wireless connection module
void setupESP8266(void) {
	// Configure the module connection
	Serial.begin(115200);   // Set the connection speed Serial
	Serial.println("AT");   // Indicate brand status in the connection through ESP8266
	delay(10);        		// Wait for response from ESP8266 module
	if (Serial.find("OK")) {	// Yes ?
		Serial.println("ESP8266 Connected !!!");
		digitalWrite(Led_Ok, HIGH);
	} 
	else {
		Serial.println("ESP8266 Crash !!!");
		digitalWrite(Led_Error, HIGH);
		return;
	}  
	// Connect Wifi through ESP8266
	digitalWrite(Led_Ok, LOW);
	Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
	delay(10);        // Wait for response from ESP8266 module
	if (Serial.find("OK")) { //  Yes answer?
		Serial.println("Wifi Connected !!!");
		digitalWrite(Led_Ok, HIGH);
		digitalWrite(Led_Error, LOW);
	} 
	else {
		Serial.println("Wifi connection error !!!");
		digitalWrite(Led_Ok, LOW);
		digitalWrite(Led_Error, HIGH);
		return;
	}
	// Open the TCP connection to the server as a client through the ESP8266 module
	digitalWrite(Led_Ok, LOW);
	Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
	delay(50);        // Wait for response from ESP8266 module
   	if (Serial.find("OK")) { // Yes answer?
		Serial.println("Connected the client to the server !!!");
		digitalWrite(Led_Ok, HIGH);
		digitalWrite(Led_Error, LOW);
	} 
	else {
		Serial.println("Could not connect to server !!!");
		digitalWrite(Led_Ok, LOW);
		digitalWrite(Led_Error, HIGH);
		return;
	}
}
// Read data from cloud and update actuator
void UpdateActuator(void) {
  	// Cycle counter
    int cyclecount; 
    // Build the HTTP call
    String httpPacket = "GET " + urlRead + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
    int length = httpPacket.length();
    // Send the package size
    Serial.print("AT+CIPSEND=");
    Serial.println(length);
    delay(10); // Wait a little for the ESP to respond if (! Serial.find (">")) return -1;
    // Send our http request
    Serial.print(httpPacket);
    delay(10); // Wait for response from ESP8266 module
    while(!Serial.available()){
      delay(5);
    }
    // Check answer ..
	if (Serial.find("\r\n\r\n")){	
	  delay(5);
      // Wait for the arrival of the reading field
  	  while (!Serial.find("\"field1\":")){
      }
      String Reading = "";
      cyclecount = 0;
      while (cyclecount < 30000) {
		if(Serial.available()) {
          int c = Serial.read(); 
        if (isDigit(c)) {	// Verify that it is a numeric value
          	Reading += (char)c; // Arm the number
          }         
        }
        cyclecount++;
      }
    
      // Update actuator output
      Actuator = Reading.toInt();   
       
	  Serial.print("Recieved from thingspeak: ");
      Serial.println(Actuator); 
      
          
    }
  Motorspeed = map(Actuator, -20, 100, 0, 255);
   // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(" ");
  lcd.print(Actuator);
  lcd.print(" degrees Centi");
  delay(100);
  analogWrite(9, Motorspeed);
}