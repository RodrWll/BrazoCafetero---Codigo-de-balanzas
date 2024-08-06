#include <Arduino.h> // Required for platformio, not needed for Arduino IDE
#include <Wire.h> // Required when using I2C devices
#include "HX711.h" // Required for HX711 load cell amplifier

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;
const double SCALE_FACTOR = -452.4247923;//Changes this value!!!
const int OFFSET = 4294906620;//Changes this value!!!
HX711 loadCell;

void setup() {
    Serial.begin(9600);
    Serial.println("Balanza calibrada");
    Serial.println("------------------------");
    loadCell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    loadCell.set_offset(OFFSET);
    loadCell.set_scale(SCALE_FACTOR);
    loadCell.tare(20);
}
void loop(){
    Serial.print("Peso: ");
    Serial.print(loadCell.get_units(), 1);
    Serial.println(" gr");
    delay(1000);
}
