# IoT Low-Energy Communication and Control

Embedded IoT experiments that connect environmental sensors, ESP8266-based network communication, ThingSpeak cloud channels, and local actuators. The repository contains two sensor-to-cloud-to-actuator control loops together with supporting seminar and hardware-engineering material.

## System overview

Two experiments are implemented:

1. **Temperature monitoring and fan control**
   - reads an analog temperature signal;
   - publishes the measured value to ThingSpeak through an ESP8266 AT-command interface;
   - retrieves the latest cloud value on a second node;
   - maps the received temperature to a PWM fan command;
   - displays the current value on a 16x2 LCD.

2. **Gas/pollution monitoring and servo control**
   - reads an analog gas-sensor signal;
   - publishes the normalized pollution level to ThingSpeak;
   - retrieves the latest cloud value on a second node;
   - maps the received value to a servo position;
   - displays the current level on a 16x2 LCD.

```text
Sensor node                         ThingSpeak                         Actuator node

Analog sensor -> Arduino -> ESP8266 ---- HTTP ----> Cloud channel ---- HTTP ----> ESP8266 -> Arduino
                    |                                                        |          |
                    +-> LCD                                                  +-> LCD    +-> Fan / Servo
```

## Repository structure

```text
.
├── .github/workflows/firmware.yml
├── firmware/
│   ├── temperature_publisher/temperature_publisher.ino
│   ├── temperature_receiver/temperature_receiver.ino
│   ├── gas_publisher/gas_publisher.ino
│   └── gas_receiver/gas_receiver.ino
├── IoT Project codes/
│   └── original Tinkercad-era sketches
├── Hardware Engineering Project/
│   ├── Counter.vhd
│   └── supporting simulation/project files
├── docs/
│   ├── architecture.md
│   └── security.md
├── Exercise_Swimlanes.pdf
├── Internet_of_Things_Low_Energy_Communication.pdf
└── README.md
```

The maintained Arduino sketches are under `firmware/`. The original sketches remain under `IoT Project codes/` as reference material from the initial simulations.

## Hardware and software components

- Arduino-compatible controller
- ESP8266 Wi-Fi module controlled with AT commands
- 16x2 LCD using the `LiquidCrystal` library
- analog temperature sensor
- analog gas sensor
- PWM-controlled fan for the temperature experiment
- servo actuator for the gas/pollution experiment
- ThingSpeak channel for cloud data exchange
- Tinkercad for the original circuit simulations
- VHDL/ModelSim material for the separate hardware-engineering exercise

## Maintained firmware

### Temperature publisher

`firmware/temperature_publisher/temperature_publisher.ino`

Reads the analog temperature input, maps it to the modeled temperature range, displays the value, and publishes it to ThingSpeak.

### Temperature receiver

`firmware/temperature_receiver/temperature_receiver.ino`

Reads the latest temperature value from ThingSpeak and maps the bounded range `-20 °C ... 100 °C` to an 8-bit PWM fan command. The receiver keeps the temperature value signed so negative values from the modeled range remain valid.

### Gas publisher

`firmware/gas_publisher/gas_publisher.ino`

Normalizes the gas-sensor reading to `0 ... 100 %`, displays the result, and publishes it to ThingSpeak.

### Gas receiver

`firmware/gas_receiver/gas_receiver.ino`

Reads the latest pollution level, constrains it to `0 ... 100 %`, and maps it to a `0 ... 180°` servo command. The servo is attached during initialization rather than on every control cycle.

## Configuration

The original simulations use the Tinkercad virtual Wi-Fi network:

```cpp
String ssid = "Simulator Wifi";
String password = "";
```

For ThingSpeak publishing, replace the placeholder in the publisher sketches with a valid write key:

```cpp
String writeApiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";
```

Active API keys must not be committed to the repository. See [`docs/security.md`](docs/security.md) for credential-handling notes.

The receiver sketches contain the ThingSpeak channel IDs used by the original public-read experiments. If a different channel is used, update the corresponding `/channels/<channel-id>/fields/1/last.json` path.

## Firmware validation

The maintained sketches are compiled automatically for an Arduino Uno through GitHub Actions using Arduino's `compile-sketches` action. The workflow is defined in `.github/workflows/firmware.yml` and runs for pull requests and pushes to `main`.

Local compilation can also be performed with Arduino CLI using the `arduino:avr:uno` board target and the `LiquidCrystal` and `Servo` libraries.

## Original simulations

- [Gas sensor and ThingSpeak simulation](https://www.tinkercad.com/things/7Ya82MAAVcd-esp2866-communicating-gas-sensor-data-with-thingspeak/editel?sharecode=MBdMVx3wsJeYs7hDtDfoMZRoxigc6fbrvtnJVhwHkk8)
- [Temperature and ThingSpeak simulation](https://www.tinkercad.com/things/cm3PAF2Fcye-esp2866-sending-and-receiving-temperature-data-to-thingspeak/editel?sharecode=HyiAJ-ycu-DPCE3ekixH3sc_7OxGZSeMEXrJf8d3Zqw)

ThingSpeak channel administration remains private to the channel owner.

## Design notes

The implementation reflects a compact embedded systems experiment rather than a production IoT stack. Communication is performed through ESP8266 AT commands and HTTP requests, which keeps the network exchange explicit in the firmware. The maintained sketches add bounded response waits, safe actuator defaults, input constraints, and clearer separation of sensor, network, display, and actuator responsibilities.

The architecture still has deliberate limitations: HTTP is unencrypted, response parsing is lightweight, and the main loop remains delay-based. A deployed implementation would normally use TLS, a non-blocking state machine, structured response parsing, watchdog-aware recovery, and stronger validation before applying cloud values to physical actuators.

Further details are documented in [`docs/architecture.md`](docs/architecture.md).

## Additional hardware-engineering material

`Hardware Engineering Project/` contains a separate automated-waste-separator exercise with VHDL counter logic and ModelSim-generated project files. It is preserved as related hardware-engineering work but is not part of the ThingSpeak IoT control loops.
