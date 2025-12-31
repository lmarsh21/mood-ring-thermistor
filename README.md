# Mood Ring Thermistor (Embedded C / AVR)

An embedded systems project that reads a thermistor through a resistor-divider circuit and visualizes temperature changes using an RGB LED. At room temperature the LED appears blue; as the thermistor warms (e.g., when touched), the LED smoothly transitions through purple toward red using PWM. Sensor readings and computed temperatures are printed over the serial port for debugging.

This project was written in **Embedded C** for an AVR/Arduino-style board using a **Makefile-based AVR toolchain** (not the Arduino `.ino` IDE workflow).

---

## Demo
🎥 **Full demo video (Google Drive):**  
https://drive.google.com/file/d/1iEVwLunAnIbOx2Ead5kvAWG_bOTkAf93/view?usp=sharing

The demo shows:
- Blue LED at room temperature  
- Smooth transition to purple/red as the thermistor is warmed by touch  

---

## How It Works

### Hardware Overview
- A **10k NTC thermistor** is connected in a voltage divider and read via the ADC.
- The ADC value is mapped to a **0–255 PWM range**.
- An RGB LED is driven using PWM:
  - Red intensity increases with temperature
  - Blue intensity decreases inversely
- Sensor values and temperatures are printed over **UART (serial)**.

### Software Overview
- **ADC input:** `analogRead(A0)`
- **PWM output:** `analogWrite()` on RGB LED pins
- **Temperature conversion:** Steinhart–Hart equation
- **Serial output:** `printf()` over UART

---

## Pin Assignments
(from `main.c`)

- **Thermistor input:** `A0`
- **RGB LED (PWM pins):**
  - Red: `6`
  - Green: `5` (initialized, not actively used in color mapping)
  - Blue: `3`
- **Dedicated flashing LED:** configurable PWM pin

> Note: If using both a green channel and a dedicated flashing LED, ensure they are assigned to different PWM-capable pins to avoid conflicts.

---

## Temperature Conversion
The thermistor resistance is calculated from the ADC reading and converted to temperature using the **Steinhart–Hart equation**:

- ADC → resistance via voltage-divider math
- Resistance → temperature (Kelvin)
- Converted to **°C and °F**

The coefficients in the code correspond to a common 10k NTC thermistor and can be adjusted for other sensors.

---

## Repository Contents
- `main.c` — Embedded C firmware source
- `Makefile` — Wrapper Makefile (includes parent course Makefile)
- `media/` — (optional) images or diagrams

---

## Build / Flash Notes
This project uses a wrapper Makefile:

```make
DEPTH = ../../
include $(DEPTH)Makefile
