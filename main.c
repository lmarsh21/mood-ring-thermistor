/* Read the voltage of a Resistor Divider Circuit and flash LED
 * while printing measurements over serial.
 */
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "unolib.h"
#include "analogRead.h"
#include "analogWrite.h"
#include "pinMode.h"
#include "delay.h"
#include "uart.h"

/* ---------- Pin assignments ---------- */
const int LED_PIN   = 5;  // dedicated flashing LED (PWM-capable pin)
const int REDLED    = 6;  // RGB red (PWM)
const int GREENLED  = 5;  // RGB green (PWM)
const int BLUELED   = 3;  // RGB blue (PWM)

/* ---------- Constants for thermistor divider ---------- */
/* Adjust these to your hardware: */
const float R_REF = 10000.0f; // reference resistor in ohms (e.g., 10k)
const float ADC_MAX = 1023.0f;

/* Steinhart–Hart coefficients for a common 10k NTC; replace with your part’s values */
const float A_SH = 0.003354016f;
const float B_SH = 0.0002569850f;
const float C_SH = 0.000002620131f;
const float D_SH = 0.00000006383091f;

/* ---------- Helpers ---------- */
static inline long clamp_long(long x, long lo, long hi){
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

long myMap(long S, long S1, long S2, long N1, long N2){
  if (S2 == S1) return N1; // avoid divide-by-zero
  long N = N1 + (long)((S - S1) * (N2 - N1) / (S2 - S1));
  return N;
}

float convertC2F(float c){
  return c * 1.8f + 32.0f;
}

/* Convert ADC code to thermistor °C using Steinhart–Hart
   Assumes divider: A0 reads V = 5V * R_th / (R_th + R_REF) with R_REF to 5V and NTC to GND,
   or the inverse depending on wiring. This version uses: R_th = R_REF * (ADC / (ADC_MAX - ADC))
*/
float ThermToDegC(int adc){
  if (adc <= 0)   adc = 1;             // avoid division by zero/log(0)
  if (adc >= 1023) adc = 1022;

  float ratio = (float)adc / (ADC_MAX - (float)adc);
  float R_th = R_REF * ratio;

  float D = logf(R_th); // Steinhart–Hart uses ln(R)
  float Tk = 1.0f / (A_SH + B_SH*D + C_SH*D*D + D_SH*D*D*D);
  return Tk - 273.15f;
}

void setLED(int red){
  // Clamp to 0..255 for PWM
  red = (int)clamp_long(red, 0, 255);
  int blue = 255 - red;
  analogWrite(REDLED, red);
  analogWrite(BLUELED, blue);
  // GREENLED unused here, but you can add it if you want a 3-channel effect
}

/* ---------- Main ---------- */
int main(void) {
  init_serial();
  pinMode(A0, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(REDLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  pinMode(BLUELED, OUTPUT);

  // Optional: set initial colors
  setLED(0);
  analogWrite(GREENLED, 0);
  analogWrite(LED_PIN, 0);

  uint8_t flash_on = 0;
  uint32_t flash_ms = 250; // flash period half-cycle (on/off each 250 ms)

  while (1) {
    int sensorValue = analogRead(A0); // 0..1023

    // Map a window (530..591) to 0..255, then clamp
    long N = myMap(sensorValue, 530, 591, 0, 255);
    N = clamp_long(N, 0, 255);

    // Compute temperature
    float therm_C = ThermToDegC(sensorValue);
    float therm_F = convertC2F(therm_C);

    // Print readings
    printf("ADC=%4d  Mapped=%3ld  T_C=%7.2f  T_F=%7.2f\r\n",
           sensorValue, N, therm_C, therm_F);

    // Drive RGB as a red/blue gradient from mapped value
    setLED((int)N);

    // Simple flash on a dedicated LED pin
    if (flash_on) {
      analogWrite(LED_PIN, 255);
    } else {
      analogWrite(LED_PIN, 0);
    }
    flash_on ^= 1;

    delay(flash_ms); // ms
  }

  return 0;
}
