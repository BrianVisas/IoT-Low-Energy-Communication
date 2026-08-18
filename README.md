# IoT Low-Energy Communication and Control

Embedded IoT experiments that connect environmental sensors, ESP8266-based network communication, ThingSpeak cloud channels, and local actuators. The repository contains two sensor-to-cloud-to-actuator control loops together with supporting seminar and hardware-engineering material.

## System overview

Two experiments are implemented:

1. **Temperature monitoring and fan control**
   - reads an analog temperature signal
   - publishes the measured value to ThingSpeak through an ESP8266 AT-command interface
   - retrieves the latest cloud value on a second node
   - maps the received temperature to a PWM fan command
   - displays the current value on a 16x2 LCD

2. **Gas/pollution monitoring and servo control**
   - reads an analog gas-sensor signal
   - publishes the normalized pollution level to ThingSpeak
   - retrieves the latest cloud value on a second node
   - maps the received value to a servo position
   - displays the current level on a 16x2 LCD

```text
Sensor node                         ThingSpeak                         Actuator node

Analog sensor -> Arduino -> ESP8266 ---- HTTP ----> Cloud channel ---- HTTP ----> ESP8266 -> Arduino
                    |                                                        |          |
                    +-> LCD                                                  +-> LCD    +-> Fan / Servo
```

## Repository structure

```text
.
├── IoT Project codes/
│   ├── _esp2866_sending_and_receiving_temperature_data_to_thingspeak1.ino
│   ├── _esp2866_sending_and_receiving_temperature_data_to_thingspeak2.ino
│   ├── esp2866_communicating_gas_sensor_data_with_thingspeak1.ino
│   └── esp2866_communicating_gas_sensor_data_with_thingspeak2.ino
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

The Arduino sketches under `IoT Project codes/` are retained as the implementation used for the original simulations. The repository documentation describes the signal flow, configuration, and limitations of those experiments.

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

## Data flow

### Temperature experiment

The publisher maps the analog temperature input to an approximate range of `-20 °C` to `100 °C` and writes the value to ThingSpeak. The receiving sketch reads the latest field value and maps it to an 8-bit PWM command for the fan.

### Gas/pollution experiment

The publisher maps the gas-sensor analog input to a normalized percentage and writes it to ThingSpeak. The receiving sketch reads the latest value and uses it as the basis for the servo position.

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

## Original simulations

- [Gas sensor and ThingSpeak simulation](https://www.tinkercad.com/things/7Ya82MAAVcd-esp2866-communicating-gas-sensor-data-with-thingspeak/editel?sharecode=MBdMVx3wsJeYs7hDtDfoMZRoxigc6fbrvtnJVhwHkk8)
- [Temperature and ThingSpeak simulation](https://www.tinkercad.com/things/cm3PAF2Fcye-esp2866-sending-and-receiving-temperature-data-to-thingspeak/editel?sharecode=HyiAJ-ycu-DPCE3ekixH3sc_7OxGZSeMEXrJf8d3Zqw)

ThingSpeak channel administration remains private to the channel owner.

## Design notes

The implementation intentionally reflects a small embedded systems experiment rather than a production IoT stack. Communication is performed through ESP8266 AT commands and HTTP requests, which keeps the protocol exchange visible and easy to inspect. The current code also uses blocking waits and simple string parsing; these choices are suitable for the original simulation but would need to be replaced by timeout-aware state machines, structured HTTP parsing, and secure transport in a deployed system.

Further architectural details are documented in [`docs/architecture.md`](docs/architecture.md).

## Additional hardware-engineering material

`Hardware Engineering Project/` contains a separate automated-waste-separator exercise with VHDL counter logic and ModelSim-generated project files. It is preserved in this repository as related hardware-engineering work but is not part of the ThingSpeak IoT control loops.
