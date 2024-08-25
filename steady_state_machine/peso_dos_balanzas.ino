#include <Arduino.h> // Required for platformio, not needed for Arduino IDE
#include <Wire.h> // Required when using I2C devices
#include "HX711.h" // Required for HX711 load cell amplifier

// HX711 circuit wiring
HX711 loadCell;
HX711 scaleCoffee;
HX711 scaleChemex;

// CONSTANTS TO FIRST SCALE - COFFEE - FIRST LOAD CELL 
const int scaleCoffee_DOUT_PIN = 4;
const int scaleCoffee_SCK_PIN = 5;
const double scaleCoffee_FACTOR = -447.76; // Change this value!!!
const int scaleCoffee_OFFSET = 617864; // Change this value!!!

// CONSTANTS TO SECOND SCALE - CHEMEX - SECOND LOAD CELL 
const int scaleChemex_DOUT_PIN = 8;
const int scaleChemex_SCK_PIN = 9;
const double scaleChemex_FACTOR = 420.4; // Change this value!!!
const int scaleChemex_OFFSET = 4294906620; // Change this value!!!

// OUTPUTS
const int weight = 9; 
// FOR WHILE WILL USE CONSTANT QUANTITIES TO TEST
const int chemex = 10, cafe_molido =20, agua_1 = 50, agua_2 = 100; // Valores de prueba 
/*TOMAR EN CUENTA
LA ESCALA LA PUSE ALEATORIAY TRABAJE CON LOS VALORES QUE LEIA
Con esta escala no lee decimales, pero si cuando aumenta 1gr; por eso los valores de prueba son de 1gr*/

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
}

// para la comunicacion con el puerto serial:
// con el Serial.read(): leemos el primer bit de un arreglo
// para eso creamos primero el arreglo a leer
String readSerialInput() {//funcion que lee lo que necesito
    String input = "";//creamos el arreglo de caracteres
    while (Serial.available() == 0) {} //No hace nada mientras no haya ningun bit por leer
    if (Serial.available() > 0) {/*
    el Serial.available: retorna el numero de bits ocupados que hay(que contienen algun caracter)*/
        input = Serial.readStringUntil('\n'); // Definimos hasta donde queremos que lea(nuestro 
        //indicador sera el enter(\n))
        input.trim(); // Remove any leading/trailing whitespace
    }
    return input;
}

void loop() {

    switch (state) {
        case 1: 
            Serial.println("\nYou are currently in state 1.");
            Serial.println("\nPlease type 'enable' to enable the system:");
            {
                String input = readSerialInput();
                if (input == "enable") {
                    Serial.println("System enabled. Moving to state 2.");
                    state=2;
                }
            }
            break;

        case 2: 
            
            Serial.print("State 2: Weighing Chemex\nPeso: ");
            gramaje = scaleCoffee.get_units(); // la balanza del chemex solo medira
            Serial.print(gramaje, 1);
            Serial.println(" gr");
            delay(1000);
            if (gramaje >= chemex) {
                Serial.println("Chemex reached the desired weight");
                digitalWrite(weight, HIGH);

                Serial.println("\nPlease type 'tare' to jump to the next state.");
                {
                    String tare_input = readSerialInput();
                    if (tare_input == "tare") {
                        Serial.println("\nMoving to state 3.");
                        scaleCoffee.set_offset(scaleCoffee_OFFSET);
                        scaleCoffee.tare(20);
                        scaleChemex.set_offset(scaleChemex_OFFSET);
                        scaleChemex.tare(20);
                        state=3;
                    }
                }
            }
            break;

        case 3:
            Serial.print("State 3: Weighing Coffee\nPeso: ");
            gramaje = scaleCoffee.get_units(); // ahora la balanza del cafe sera la que mida
            Serial.print(gramaje, 1);
            Serial.println(" gr");
            delay(1000);
            if (gramaje >= cafe_molido) {
                Serial.println("Coffee reached the desired weight");
                digitalWrite(weight, HIGH);
                Serial.println("\nPlease type 'tare' to jump to the next state.");
                {
                    String tare_input2 = readSerialInput();
                    if (tare_input2 == "tare") {
                        Serial.println("\nMoving to state 4.");
                        scaleCoffee.set_offset(scaleCoffee_OFFSET);
                        scaleCoffee.tare(20);
                        scaleChemex.set_offset(scaleChemex_OFFSET);
                        scaleChemex.tare(20);
                        state=4;
                    }
                }
            }
            break;

        case 4:
            Serial.print("State 4: Weighing Water 1\nPeso: ");
            gramaje = scaleCoffee.get_units();
            Serial.print(gramaje, 1);
            Serial.println(" gr");
            delay(1000);
            if (gramaje >= agua_1) {
                Serial.println("Water 1 reached the desired weight");
                digitalWrite(weight, HIGH);
                Serial.println("\nPlease type 'tare' to jump to the next state.");
                {
                    String tare_input3 = readSerialInput();
                    if (tare_input3 == "tare") {
                        Serial.println("\nMoving to state 5.");
                        scaleCoffee.set_offset(scaleCoffee_OFFSET);
                        scaleCoffee.tare(20);
                        scaleChemex.set_offset(scaleChemex_OFFSET);
                        scaleChemex.tare(20);
                        state=5;
                    }
                }
            }
            break;

        case 5:
            Serial.print("State 5: Weighing Water 2\nPeso: ");
            gramaje = scaleCoffee.get_units();
            Serial.print(gramaje, 1);
            Serial.println(" gr");
            delay(1000);
            if (gramaje >= agua_2) {
                Serial.println("Water 2 reached the desired weight");
                digitalWrite(weight, HIGH);
                {
                    String tare_input4 = readSerialInput();
                    if (tare_input4 == "tare") {
                        Serial.println("\nProcess finished.");
                        scaleCoffee.set_offset(scaleCoffee_OFFSET);
                        scaleCoffee.tare(20);
                        scaleChemex.set_offset(scaleChemex_OFFSET);
                        scaleChemex.tare(20);
                        
                    }
                }
            }
            break;

        default:
            Serial.println("Unknown state.");
            break;
    }
}
