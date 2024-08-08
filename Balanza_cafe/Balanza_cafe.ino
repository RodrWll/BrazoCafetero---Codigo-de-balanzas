#include <Arduino.h> // Required for platformio, not needed for Arduino IDE
#include <Wire.h> // Required when using I2C devices
#include "HX711.h" // Required for HX711 load cell amplifier
#include <Pushbutton.h>
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;
const double SCALE_FACTOR = -454.7109308;//Changes this value!!!
const int OFFSET = 4294906705;//Changes this value!!!
HX711 loadCell;
const int buttonPin = 3;

void setup() {
    Serial.begin(9600);
    Serial.println("Balanza calibrada");
    Serial.println("------------------------");
    loadCell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    loadCell.set_offset(OFFSET);
    loadCell.set_scale(SCALE_FACTOR);
    loadCell.tare(20);
    pinMode(buttonPin,INPUT);
    attachInterrupt(digitalPinToInterrupt(buttonPin), interrupt, RISING); //(pin de interrupcion, funcion que se pide ejecutar al presionar el boton, modo de trigger)

}

//funcion de interrupcion
void interrupt(){
  loadCell.set_offset(OFFSET);
  loadCell.tare(20);
  Serial.println("\nTare...pls wait");
}

void loop(){
    Serial.print("Peso: ");
    Serial.print(loadCell.get_units(), 1);
    Serial.println(" gr");
    delay(1000);
}
