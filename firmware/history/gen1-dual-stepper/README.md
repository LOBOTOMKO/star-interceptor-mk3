# firmware/history/gen1-dual-stepper/

The original Generation 1 firmware, kept here as a record of the project's evolution (see the main [README](../../../README.md#the-build--three-generations-each-solving-the-last-ones-failure)).

In this generation **both axes — azimuth and elevation — were driven by stepper motors**, each stepped directly through a 4-pin H-bridge sequence (no microstepping driver yet). Target coordinates arrived over Bluetooth Low Energy (`main.ino`) and were converted into step counts for MOTOR1 (azimuth) and MOTOR2 (elevation).

- `main.ino` — the BLE-driven dual-stepper controller: receives a `latitude longitude` pair, converts it into step counts, and drives both H-bridge steppers.
- `rxtx1.ino` — an earlier bring-up sketch exercising the elevation and azimuth H-bridge pin sequences directly, before the BLE layer was added.

**Why it was replaced:** with the H-bridge drivers powered down between moves, the telescope's own weight back-drove the elevation motor and it slipped off target, along with vibration, jitter and audible noise during stepping. Generation 2 moved elevation to a metal-geared servo (MG996R) with position feedback to fix the back-driving; Generation 3 (the current firmware in `firmware/`) returns elevation to a stepper, but now through a worm gear, which can't be back-driven.
