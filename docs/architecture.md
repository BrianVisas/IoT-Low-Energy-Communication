# Architecture

## Overview

The IoT experiments use a two-node pattern: a sensor node publishes a measured value to ThingSpeak, and an actuator node reads the latest cloud value and applies a local control output.

```text
+------------------+        +-------------------+        +------------------+
| Sensor node      |        | ThingSpeak        |        | Actuator node    |
|                  |        |                   |        |                  |
| Analog sensor    |        | Field 1           |        | ESP8266          |
| Arduino          |--HTTP->| Latest value      |--HTTP->| Arduino          |
| ESP8266          |        |                   |        | LCD              |
| LCD              |        |                   |        | Fan / Servo      |
+------------------+        +-------------------+        +------------------+
```

The ESP8266 is controlled through AT commands over the Arduino serial interface. This makes the Wi-Fi association, TCP setup, HTTP request generation, and server response handling explicit in the firmware.

## Temperature control loop

### Publisher

1. Read the analog input on `A0`.
2. Map the raw ADC value to an approximate temperature range.
3. Display the temperature on the LCD.
4. Build a ThingSpeak `/update` HTTP request.
5. Send the request through the ESP8266 TCP connection.

### Receiver

1. Request the latest value from the configured ThingSpeak channel.
2. Parse `field1` from the returned payload.
3. Convert the value to an integer.
4. Map the temperature range to an 8-bit PWM value.
5. Drive the fan from PWM output pin 9.
6. Display the received temperature on the LCD.

## Gas/pollution control loop

### Publisher

1. Read the analog gas-sensor input on `A0`.
2. Map the sensor range to a normalized percentage.
3. Display the percentage on the LCD.
4. Publish the value to ThingSpeak.

### Receiver

1. Request the latest gas/pollution value from ThingSpeak.
2. Parse `field1` from the response.
3. Convert the received value to an actuator command.
4. Apply the command to the servo on pin 9.
5. Display the current pollution level on the LCD.

## Communication sequence

```text
Arduino             ESP8266                 ThingSpeak
   |                    |                        |
   | AT                 |                        |
   |------------------->|                        |
   | AT+CWJAP           |                        |
   |------------------->|                        |
   | AT+CIPSTART        |----------------------->|
   |                    |     TCP connection     |
   | AT+CIPSEND         |                        |
   |------------------->|                        |
   | HTTP GET           |----------------------->|
   |                    |                        |
   |                    |<-----------------------|
   |<-------------------|     HTTP response      |
```

## Pin usage

The sketches use the following main pin assignments:

| Function | Pin / interface |
|---|---|
| Analog sensor input | `A0` |
| LCD RS | `12` |
| LCD Enable | `11` |
| LCD data | `5, 4, 3, 2` |
| Fan PWM | `9` |
| Servo signal | `9` |
| ESP8266 communication | Arduino serial interface |

The fan and servo belong to separate experiments and therefore reuse pin 9.

## Current limitations

The original firmware is intentionally simple and has several constraints:

- communication is blocking and relies on fixed delays;
- response parsing uses string searches rather than a structured HTTP/JSON parser;
- retry and timeout handling is minimal;
- the ESP8266 connection is established through AT commands rather than a dedicated networking library;
- HTTP is used instead of TLS-protected HTTPS;
- actuator commands are derived directly from cloud values without filtering, hysteresis, or plausibility checks.

For a deployed embedded system, these areas would normally be addressed with a non-blocking state machine, bounded timeouts, explicit error states, secure transport, input validation, and watchdog-aware recovery behavior.
