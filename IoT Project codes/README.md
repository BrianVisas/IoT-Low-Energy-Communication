# Original IoT sketches

This directory contains the sketches used for the initial Tinkercad/ThingSpeak experiments.

The maintained implementations are under [`../firmware`](../firmware), where the sensor publishers and actuator receivers are separated into dedicated sketch directories and include additional error handling, bounded waits, input constraints, and credential placeholders.

The original publisher sketches in this directory have also been sanitized so active ThingSpeak write keys are not stored in the current source tree.
