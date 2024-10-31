#include <Arduino.h> // Required for platformio, not needed for Arduino IDE
#include <Wire.h>    // Required when using I2C devices
#include "HX711.h"   // Required for HX711 load cell amplifier
#include <LiquidCrystal_I2C.h>
#include "Servo.h"

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int buttonPin2 = 11; // Pin al que está conectado el botón
int buttonState = 0;       // Estado actual del botón
int lastButtonState = 0;   // Último estado del botón
int toggleState = 0;       // Estado para alternar entre mensajes
int mensaje = 0;
int caffe_total;

// Potenciómetro
int potPin = A1;      // Pin del potenciómetro
int lastPotValue = 0; // Valor leído del potenciómetro
int mappedValue = 0;  // Valor mapeado entre 10 y 100
int potValue = 0;

// HX711 circuit wiring
HX711 scaleCoffee;
HX711 scaleChemex;
// CONSTANTS TO FIRST SCALE - COFFE
const int scaleCoffee_DOUT_PIN =8;
const int scaleCoffee_SCK_PIN =9;
const double scaleCoffee_FACTOR = 420.4; // Change this value!!!
const int scaleCoffee_OFFSET = 4294906620;     // Change this value!!!
// CONSTANTS TO SECOND SCALE - CHEMEX
const int scaleChemex_DOUT_PIN = 4;
const int scaleChemex_SCK_PIN = 5;
const double scaleChemex_FACTOR = -447.76;   // Change this value!!!
const int scaleChemex_OFFSET = 617864; // Change this value!!!

// INPUTS PIN
const int tare_DOUT_PIN = 3;
const int enable_DOUT_PIN = 7;
// OUTPUTS PIN
const int weigth_DIN_PIN = 12;

// Internal variables
int chemex = 400 /*400 pesa el chemex*/, cafe_molido = 20, agua_1 = 20, agua_2 = 20; // Valores de prueba
int state = 1;
float gramaje = 0;

// Botón para resetear estados
const int buttonPin = 2; // Pin del botón
int buttonState2 = 0;
int lastButtonState2 = 0;

// Mecanismo de dispensación
Servo servoMecha;
const int pinServo = 10;
const int angMax = 70;
int grUmbral;
int ang;
const int dispensador_en = 6; // Preliminar para activar el dispensador

bool flagServo = false;

void setup()
{
    delay(500);
    Serial.begin(9600);
    Serial.println("Starting...");
    Serial.println("------------------------");
    // boton 2
    pinMode(buttonPin2, INPUT_PULLUP);
    // Configuración del botón con interrupción
    pinMode(buttonPin, INPUT_PULLUP);
    // Inicializar balanzas
    scaleCoffee.begin(scaleCoffee_DOUT_PIN, scaleCoffee_SCK_PIN);
    scaleCoffee.set_offset(scaleCoffee_OFFSET);
    scaleCoffee.set_scale(scaleCoffee_FACTOR);
    scaleCoffee.tare(20);
    scaleChemex.begin(scaleChemex_DOUT_PIN, scaleChemex_SCK_PIN);
    scaleChemex.set_offset(scaleChemex_OFFSET);
    scaleChemex.set_scale(scaleChemex_FACTOR);
    scaleChemex.tare(20);
    // señales
    pinMode(weigth_DIN_PIN, OUTPUT);
    pinMode(dispensador_en, OUTPUT);
    pinMode(tare_DOUT_PIN, INPUT_PULLUP);
    pinMode(enable_DOUT_PIN, INPUT_PULLUP);
    pinMode(buttonPin, INPUT_PULLUP); // Configura el botón como entrada
    // inicializar lcd
    lcd.init();      // Inicializa el LCD
    lcd.backlight(); // Enciende la retroiluminación del LCD
    lcd.setCursor(0, 0);
    // Servo del mecanismo
    servoMecha.attach(pinServo);
    // Set angle to 0
    servoMecha.write(0);
}

int tare_function()
{
    scaleCoffee.set_offset(scaleCoffee_OFFSET);
    scaleCoffee.tare(20);
    scaleChemex.set_offset(scaleChemex_OFFSET);
    scaleChemex.tare(20);
    Serial.println("Tare Signal Received...pls wait");
    return state = state + 1;
}

void coffeeMessage()
{ // del cafe
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("COFFEE ");
    lcd.print(" Gr:");
    lcd.print(gramaje);
    lcd.setCursor(0, 1); // comlumnas/filas
    lcd.print("St:");
    lcd.print(state);
    lcd.print("|En: ");
    lcd.print(digitalRead(enable_DOUT_PIN));
    lcd.print("|Ta: ");
    lcd.print(digitalRead(tare_DOUT_PIN));
}

void chemexMessage()
{ // del chemex
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CHEMEX ");
    lcd.print(" Gr:");
    lcd.print(gramaje);
    lcd.setCursor(0, 1); // comlumnas/filas
    lcd.print("St:");
    lcd.print(state);
    lcd.print("|En: ");
    lcd.print(digitalRead(enable_DOUT_PIN));
    lcd.print("|Ta: ");
    lcd.print(digitalRead(tare_DOUT_PIN));
}

void resetButtonAction()
{
    buttonState2 = digitalRead(buttonPin);
    if (buttonState2 != lastButtonState2)
    {
        lastButtonState2 = buttonState2;
        if (buttonState2 == LOW)
        { // Si el botón fue presionado
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("-----RESET------ ");
            state = 1; // reseteo el estado y regreso al estado anterior
            delay(900);
        }
        delay(50);
    }
}

void toggleScreenButtonAction()
{
    buttonState = digitalRead(buttonPin2);
    if (buttonState != lastButtonState)
    {
        lastButtonState = buttonState;
        if (buttonState == LOW)
        {                               // Si el botón fue presionado
            toggleState = !toggleState; // Cambia el estado de la variable toggleState//empieza en 0
            if (toggleState)
            {
                mensaje = 2; // Muestra el segundo mensaje
            }
            else
            {
                mensaje = 1; // Vuelve a mostrar el primer mensaje
            }
        }
        delay(50);
    }
}

void loop()
{
    resetButtonAction();
    toggleScreenButtonAction();
    // POTENCIOMETRO
    potValue = analogRead(potPin);
    mappedValue = map(potValue, 0, 1023, 0, 50);
    if (abs(lastPotValue - mappedValue) > 5) // Change the value if the difference is greater than 2
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Valor cafe: ");
        caffe_total = cafe_molido + mappedValue;
        lcd.print(caffe_total);
        lcd.setCursor(0, 1);
        delay(100);
    }
    lastPotValue = mappedValue;
    // estados
    switch (state)
    {
    case 1:
    {
        digitalWrite(weigth_DIN_PIN, LOW);
        chemexMessage();
        if (mensaje == 1)
        {
            chemexMessage();
        }
        else if (mensaje == 2)
        {
            coffeeMessage();
        }

        int digital_enable = digitalRead(enable_DOUT_PIN);
        if (digital_enable == 1)
        { // ESPERARA AL BRAZO PARA EMPEZAR A MEDIR JUNTO AL DISPENSADOR
            tare_function();
        }
    }
    break;
    case 2:
    {
        digitalWrite(weigth_DIN_PIN, LOW);
        Serial.print("State 2: Weighing Chemex\nPeso: ");
        gramaje = scaleChemex.get_units(); // la balanza del chemex solo medira
        Serial.print(gramaje, 1);
        Serial.println(" gr");
        delay(200);
        chemexMessage(); // First show the chemex message because this state is for the chemex
        if (mensaje == 1)
        {
            chemexMessage();
        }
        else if (mensaje == 2)
        {
            coffeeMessage();
        }
        if (gramaje >= chemex)
        {
            Serial.println("Chemex reached the desired weight");
            digitalWrite(weigth_DIN_PIN, HIGH);

            int tare_digital = digitalRead(tare_DOUT_PIN);

            if (tare_digital == 1)
            {
                tare_function();
            }
        }
    }
    break;
    case 3:
    {
        digitalWrite(dispensador_en, HIGH);
        digitalWrite(weigth_DIN_PIN, LOW);
        Serial.print("State 3: Weighing Coffee\nPeso: ");
        gramaje = scaleCoffee.get_units(); // ahora la balanza del cafe sera la que mida
        Serial.print(gramaje, 1);
        Serial.println(" gr");
        delay(200);
        coffeeMessage();
        if (mensaje == 1)
        {
            chemexMessage();
        }
        else if (mensaje == 2)
        {
            coffeeMessage();
        }
        grUmbral = caffe_total - 10;
        if (gramaje < grUmbral && !flagServo)
        {
            servoMecha.write(angMax); // In this case only the servo will be activated once
            flagServo = true;
        }
        else if (gramaje >= grUmbral)
        {
            ang = angMax - 7.5 * (gramaje - grUmbral);
            servoMecha.write(ang);
            flagServo = false; // Resetear la bandera
        }
        if (gramaje >= caffe_total)
        {
            servoMecha.write(0);
        }
        if (gramaje >= caffe_total)
        {
            Serial.println("Coffee reached the desired weight");
            digitalWrite(weigth_DIN_PIN, HIGH);
            Serial.println("\nWaiting for the signal tare to jump to the next state.");
            {
                int tare_digital = digitalRead(tare_DOUT_PIN);
                if (tare_digital == 1)
                {
                    tare_function();
                }
            }
        }
    }
    break;
    case 4:
    {
        digitalWrite(weigth_DIN_PIN, LOW);
        Serial.print("State 4: Weighing Water 1\nPeso: ");
        gramaje = scaleChemex.get_units();
        Serial.print(gramaje, 1);
        Serial.println(" gr");
        delay(200);
        chemexMessage();
        if (mensaje == 1)
        {
            chemexMessage();
        }
        else if (mensaje == 2)
        {
            coffeeMessage();
        }
        if (gramaje >= agua_1)
        {
            Serial.println("Water 1 reached the desired weight");
            digitalWrite(weigth_DIN_PIN, HIGH);
            Serial.println("\nWaiting for the signal tare to jump to the next state.");
            {
                int tare_digital = digitalRead(tare_DOUT_PIN);
                if (tare_digital == 1)
                {
                    tare_function();
                }
            }
        }
    }
    break;
    case 5:
    {
        digitalWrite(weigth_DIN_PIN, LOW);
        Serial.print("State 5: Weighing Water 2\nPeso: ");
        gramaje = scaleChemex.get_units();
        Serial.print(gramaje, 1);
        Serial.println(" gr");
        delay(200);
        chemexMessage();
        if (mensaje == 1)
        {
            chemexMessage();
        }
        else if (mensaje == 2)
        {
            coffeeMessage();
        }
        if (gramaje >= agua_2)
        {
            Serial.println("Water 2 reached the desired weight");
            digitalWrite(weigth_DIN_PIN, HIGH);
            Serial.println("\nProcess finished. Pls write disable to deactivate");
            {
                int tare_digital = digitalRead(tare_DOUT_PIN);
                if (tare_digital == 1)
                {
                    if (digitalRead(enable_DOUT_PIN) == 0)
                    {
                        lcd.print("END");
                        state = 6;
                    }
                }
            }
        }
    }
    break;
    case 6:
    {
        // No operation for state 6
    }
    break;
    default:
    {
        Serial.println("Unknown state.");
    }
    break;
    }
}