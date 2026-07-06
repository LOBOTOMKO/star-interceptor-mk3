# firmware/

ESP32 firmware (C/C++).

Handles the real-time work on the mount:
- receives target orientations over Bluetooth Low Energy
- runs the coordinate → axis-angle logic
- drives the stepper motor(s) with the S-curve acceleration profile
- (v2) tracks an object autonomously from a buffered orientation array

**Add here:** the ESP32 source (`.ino` / `.cpp` / `.h`) and any PlatformIO/Arduino project files.
