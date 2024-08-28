#include <Arduino.h> // Required for platformio, not needed for Arduino IDE
#include <Wire.h> // Required when using I2C devices
#include "HX711.h" // Required for HX711 load cell amplifier

// HX711 circuit wiring
HX711 loadCell;
HX711 scaleCoffee;
HX711 scaleChemex;

// CONSTANTS TO FIRST SCALE - COFFEE 
const int scaleCoffee_DOUT_PIN = 4;
const int scaleCoffee_SCK_PIN = 5;
const double scaleCoffee_FACTOR = -447.76; // Change this value!!!
const int scaleCoffee_OFFSET = 617864; // Change this value!!!

// CONSTANTS TO SECOND SCALE - CHEMEX 
const int scaleChemex_DOUT_PIN = 8;
const int scaleChemex_SCK_PIN = 9;
const double scaleChemex_FACTOR = 420.4; // Change this value!!!
const int scaleChemex_OFFSET = 4294906620; // Change this value!!!
// INPUTS PIN
const int tare = 3; 
const int enable = 7; 
// OUTPUTS PIN 
const int weight = 6; 
// FOR WHILE WILL USE CONSTANT QUANTITIES TO TEST
const int chemex = 20, cafe_molido =100, agua_1 = 200, agua_2 = 50; // Valores de prueba 
// Internal variables
int state = 1;
int gramaje = 0;

void setup() {
    Serial.begin(9600);
    Serial.println("Starting...");
    Serial.println("------------------------");
    
    // Initialize scales
    scaleCoffee.begin(scaleCoffee_DOUT_PIN, scaleCoffee_SCK_PIN);
    scaleCoffee.set_offset(scaleCoffee_OFFSET);
    scaleCoffee.set_scale(scaleCoffee_FACTOR);
    scaleCoffee.tare(20);

    scaleChemex.begin(scaleChemex_DOUT_PIN, scaleChemex_SCK_PIN);
    scaleChemex.set_offset(scaleChemex_OFFSET);
    scaleChemex.set_scale(scaleChemex_FACTOR);
    scaleChemex.tare(20);

    pinMode(weight, OUTPUT);
    pinMode(tare, INPUT_PULLUP);
    pinMode(enable, INPUT_PULLUP);
}
int tare_function(){
    scaleCoffee.set_offset(scaleCoffee_OFFSET);
    scaleCoffee.tare(20);
    scaleChemex.set_offset(scaleChemex_OFFSET);
    scaleChemex.tare(20);
    Serial.println("Tare Signal Received...pls wait");
    return state=state+1;
    
}

void loop() {

    switch (state) {
       
       case 1: 
            digitalWrite(weight, LOW);
            Serial.println("\nYou are currently in state 1.");
            Serial.println("\nWaiting for the signal tare to jump to the next state.");
            delay(1000);
            Serial.println("Enable: ");
            Serial.print(digitalRead(enable));
            Serial.println("  |  ");
            Serial.print("Tare: ");
            Serial.print(digitalRead(tare));
            delay(1000);
            {
              int digital_enable = digitalRead(enable);
               // String input = readSerialInput();
                if (digital_enable== 1) {
                    Serial.println("Enable: ");
                    Serial.print(digitalRead(enable));
                    Serial.println("  |  ");
                    Serial.print("Tare: ");
                    Serial.print(digitalRead(tare));
                    Serial.println("System enabled. ");
                    Serial.println("\nMoving to state 2.");
                    tare_function();
                    
                    
                }
            }
            break;

        case 2: 
            digitalWrite(weight, LOW);
            Serial.print("State 2: Weighing Chemex\nPeso: ");
            gramaje = scaleCoffee.get_units(); // la balanza del chemex solo medira
            Serial.print(gramaje, 1);
            Serial.println(" gr");
            delay(1000);
            if (gramaje >= chemex) {
                Serial.println("Chemex reached the desired weight");
                digitalWrite(weight, HIGH);

                Serial.println("\nWaiting for the signal tare to jump to the next state.");
                {
                  int tare_digital = digitalRead(tare);
                    //String tare_input = readSerialInput();
                    if (tare_digital== 1) {
                        Serial.println("\nMoving to state 3.");
                        Serial.println("Enable: ");
                        Serial.print(digitalRead(enable));
                        Serial.println("  |  ");
                        Serial.print("Tare: ");
                        Serial.print(digitalRead(tare));
                        tare_function();
                    }
                }
            }
            break;

        case 3:
           digitalWrite(weight, LOW);
            Serial.print("State 3: Weighing Coffee\nPeso: ");
            gramaje = scaleChemex.get_units(); // ahora la balanza del cafe sera la que mida
            Serial.print(gramaje, 1);
            Serial.println(" gr");
            delay(1000);
            if (gramaje >= cafe_molido) {
                Serial.println("Coffee reached the desired weight");
                digitalWrite(weight, HIGH);
                Serial.println("\nWaiting for the signal tare to jump to the next state.");
                {
                     int tare_digital = digitalRead(tare);
                    //String tare_input = readSerialInput();
                    if (tare_digital== 0) {
                        Serial.println("\nMoving to state 4.");
                        Serial.println("Enable: ");
                        Serial.print(digitalRead(enable));
                        Serial.println("  |  ");
                        Serial.print("Tare: ");
                        Serial.print(digitalRead(tare));
                        tare_function();
                    }
                }
            }
            break;

        case 4:
            digitalWrite(weight, LOW);
            Serial.print("State 4: Weighing Water 1\nPeso: ");
            gramaje = scaleChemex.get_units();
            Serial.print(gramaje, 1);
            Serial.println(" gr");
            delay(1000);
            if (gramaje >= agua_1) {
                Serial.println("Water 1 reached the desired weight");
                digitalWrite(weight, HIGH);
                Serial.println("\nWaiting for the signal tare to jump to the next state.");
                {
                     int tare_digital = digitalRead(tare);
                    //String tare_input = readSerialInput();
                    if (tare_digital== 1) {
                        Serial.println("\nMoving to state 5.");
                        Serial.println("Enable: ");
                        Serial.print(digitalRead(enable));
                        Serial.println("  |  ");
                        Serial.print("Tare: ");
                        Serial.print(digitalRead(tare));
                        tare_function();
                    }
                }
            }
            break;

        case 5:
            digitalWrite(weight, LOW);
            Serial.print("State 5: Weighing Water 2\nPeso: ");
            gramaje = scaleChemex.get_units();
            Serial.print(gramaje, 1);
            Serial.println(" gr");
            delay(1000);
            if (gramaje >= agua_2) {
                Serial.println("Water 2 reached the desired weight");
                digitalWrite(weight, HIGH);
                Serial.println("\nProcess finished. Pls write disable to deactivate");
                {
                     int tare_digital = digitalRead(tare);
                    //String tare_input = readSerialInput();
                    if (tare_digital== 1) {
                        Serial.println("\nProcces finished. Waiting to die :c");
                        Serial.println("Enable: ");
                        Serial.print(digitalRead(enable));
                        Serial.println("  |  ");
                        Serial.print("Tare: ");
                        Serial.print(digitalRead(tare));
                        if (digitalRead(enable) == 0) {
                        Serial.print("Enable: ");
                        Serial.print(digitalRead(enable));
                        Serial.println("  |  ");
                        Serial.print("Tare: ");
                        Serial.print(digitalRead(tare));
                        Serial.println("\nBye:c...");
                        state=6;
                        
                    }
                    }
                }
            }
            break;
        
        case 6:
            break;
        default:
            Serial.println("Unknown state.");
            break;
    }
}
