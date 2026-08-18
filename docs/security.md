# Security and configuration

## API keys

ThingSpeak write API keys are credentials and must not be committed to source control. Publisher sketches use the placeholder:

```cpp
String writeApiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";
```

Replace the placeholder only in a local working copy or load the value from a local configuration file that is excluded from version control.

Two write keys were present in earlier revisions of this public repository. Because a committed credential must be considered exposed even after it is removed from the current source tree, those keys should be revoked or regenerated in ThingSpeak before reuse.

## Public channel identifiers

The receiver sketches use ThingSpeak channel identifiers to read the latest value from a public field. Channel IDs used for public reads are not treated as secrets, but changing the experiment to a private channel requires appropriate read-key handling.

## Network transport

The original sketches communicate with `api.thingspeak.com` over TCP port 80 using HTTP. This matches the original educational simulation, but it does not provide transport encryption. A deployed implementation should use TLS-capable networking and HTTPS, together with certificate validation appropriate to the target platform.

## Wi-Fi credentials

The checked-in sketches reference the Tinkercad simulation network (`Simulator Wifi`) and an empty password. Real SSIDs and passwords should be stored outside tracked source files.

## General guidance

- do not commit active API keys, Wi-Fi passwords, tokens, or private certificates;
- rotate any credential that has previously appeared in public history;
- prefer separate development and production credentials;
- validate cloud responses before applying them to physical actuators;
- use bounded timeouts and safe actuator defaults on communication failure.
