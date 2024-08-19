#include <Arduino.h> // Required for platformio, not needed for Arduino IDE
#include <Wire.h> // Required when using I2C devices
#include "HX711.h" // Required for HX711 load cell amplifier
#include <Pushbutton.h>
// HX711 circuit wiring
HX711 loadCell;
HX711 scaleCoffee;
HX711 scaleChemex;
//CONSTANTS TO FIRST SCALE - COFFEE

const int scaleCoffee_DOUT_PIN = 4;
const int scaleCoffee_SCK_PIN = 5;
const double scaleCoffee_FACTOR = -454.7109308;//Changes this value!!!
const int scaleCoffee_OFFSET = 4294906705;//Changes this value!!!

//CONSTANTS TO SECOND SCALE - CHEMEX 
const int scaleChemex_DOUT_PIN = 6;
const int scaleChemex_SCK_PIN = 7;
const double scaleChemex_FACTOR = -454.7109308;//Changes this value!!!
const int scaleChemex_OFFSET = 4294906705;//Changes this value!!!


//INPUTS
// External interrupt pins are mapped to pin 2 and pin 3 ARDUINO NANO
const int pinTare = 3;
const int enable = 4;
//Internal variables
int state=0;
int gramaje=0;

//OUTPUTS
int weight=0; 
//FOR WHILE WILL USE CONSTANT QUANTITIES TO TEST
const int chemex=300,cafe_molido=50,agua_1=200,agua_2=100; //valores de prueba con lo que tenemos



void setup() {
    Serial.begin(9600);
    Serial.println("Starting...");
    Serial.println("------------------------");
    // Initialize scales, first scale is for coffee, second scale is for chemex
    scaleCoffee.begin(scaleCoffee_DOUT_PIN, scaleCoffee_SCK_PIN);
    scaleCoffee.set_scale(scaleCoffee_FACTOR);
    scaleCoffee.set_offset(scaleCoffee_OFFSET);
    scaleCoffee.tare(20);
    // Initialize second scale
    scaleChemex.begin(scaleChemex_DOUT_PIN, scaleChemex_SCK_PIN);
    scaleChemex.set_scale(scaleChemex_FACTOR);
    scaleChemex.set_offset(scaleChemex_OFFSET);
    scaleChemex.tare(20);

    pinMode(pinTare,INPUT);
    //attachInterrupt(digitalPinToInterrupt(pinTare), interrupt, RISING); //(pin de interrupcion, funcion que se pide ejecutar al presionar el boton, modo de trigger)

}

//funcion de interrupcion
/*void interrupt(){
  loadCell.set_offset(OFFSET);
  loadCell.tare(20);
  Serial.println("\nTare...pls wait");
  estado+=1;
}*/

//This program is a state machine that will be used to weight the ingredients of a coffee recipe
void loop(){
    //save logical value of the tare 
    int tare=digitalRead(pinTare);//xArm send tare signal 
    
    if (digitalRead(pinTare)==1){
        scaleCoffee.set_offset(OFFSET);//Ani, please check this. Is necessary to set the offset to the scale?
        scaleCoffee.tare(20);
        scaleChemex.set_offset(OFFSET);
        scaleChemex.tare(20);
        Serial.println("\nTare Signal Received...pls wait");
        state+=1;
    }
    
    if (state==0)//INICIAR
    {
        Serial.println("\n...pls wait");
        Serial.println("\nWe are in state 0");
    }else if (state==2){
        Serial.print("\nState 2: Weighing Chemex");
        Serial.print("\nPeso: ");
        gramaje = scaleChemex.get_units()
        Serial.print(gramaje, 1);
        Serial.println(" gr");
        //delay(1000); 
        if (gramaje == chemex)//maybe we need to add a tolerance
        {
            Serial.println("\nChemex reach the desired weight");
            digitalWrite(weight, HIGH);//Send output to xArm
            //xArm will send Tare signal and we will go to the next state
        }
    }else if (state==3){
        Serial.print("\nState 3: Weighing Coffee");
        Serial.print("\nPeso: ");
        gramaje = scaleCoffee.get_units(), 1;
        Serial.print(gramaje,1);
        Serial.println(" gr");
        if (gramaje==cafe_molido)//maybe we need to add a tolerance
        {
            Serial.println("\nCoffee reach the desired weight");
            digitalWrite(weight, HIGH);//Send output to xArm
            //xArm will send Tare signal and we will go to the next state
        }
    }else if (state==4){
        Serial.print("\nState 4: Weighing Water 1");
        Serial.print("\nPeso: ");
        gramaje = scaleCoffee.get_units(), 1;
        Serial.print(gramaje,1);
        Serial.println(" gr");
        if (gramaje==agua_1)//maybe we need to add a tolerance
        {
            Serial.println("\nWater 1 reach the desired weight");
            digitalWrite(weight, HIGH);//Send output to xArm
            //xArm will send Tare signal and we will go to the next state
        }
    }else if (state==5){
        Serial.print("\nState 5: Weighing Water 2");
        Serial.print("\nPeso: ");
        gramaje = scaleCoffee.get_units(), 1;
        Serial.print(gramaje,1);
        Serial.println(" gr");
        if (gramaje==agua_2)//maybe we need to add a tolerance
        {
            Serial.println("\nWater 2 reach the desired weight");
            digitalWrite(weight, HIGH);//Send output to xArm
            //xArm will send Tare signal and we will go to the next state
        }
    }
}