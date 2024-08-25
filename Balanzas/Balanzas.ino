#include <Arduino.h> // Required for platformio, not needed for Arduino IDE
#include <Wire.h> // Required when using I2C devices
#include "HX711.h" // Required for HX711 load cell amplifier

//CAFE //TA BIEN 
const int scaleCoffee_DOUT_PIN = 6;
const int scaleCoffee_SCK_PIN = 7;
const double scaleCoffee_FACTOR = -447.76; // Change this value!!!
const int scaleCoffee_OFFSET =  617864; // Change this value!!!
//CHEMEX//TA BIEN? XDDDDDDDDDDDDDD
const int scaleChemex_DOUT_PIN = 4;
const int scaleChemex_SCK_PIN = 5;
const double scaleChemex_FACTOR = 420; // Change this value!!!
const int scaleChemex_OFFSET = 4294906620; // Change this value!!!
// HX711 circuit wiring
HX711 loadCell;
HX711 scaleCoffee;
HX711 scaleChemex;

void setup() {
    Serial.begin(9600);
    Serial.println("Balanza calibrada");
    scaleCoffee.begin(scaleCoffee_DOUT_PIN, scaleCoffee_SCK_PIN);
    scaleCoffee.set_offset(scaleCoffee_OFFSET);
    scaleCoffee.set_scale(scaleCoffee_FACTOR);
    scaleCoffee.tare(20);

    scaleChemex.begin(scaleChemex_DOUT_PIN, scaleChemex_SCK_PIN);
    scaleChemex.set_offset(scaleChemex_OFFSET);
    scaleChemex.set_scale(scaleChemex_FACTOR);
    scaleChemex.tare(20);
}
void loop(){
    Serial.print("\nPeso chemex : ");
    Serial.print(scaleChemex.get_units(), 1);
    Serial.println(" gr");
    delay(1500);
    Serial.print("Peso cofee: ");//TA BIEN 
    Serial.print(scaleCoffee.get_units(), 1);
    Serial.println(" gr");
    delay(1500);
}
