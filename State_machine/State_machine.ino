#include <Arduino.h> // Required for platformio, not needed for Arduino IDE
#include <Wire.h>    // Required when using I2C devices
#include "HX711.h"   // Required for HX711 load cell amplifier
#include <LiquidCrystal_I2C.h>
#include "Servo.h"
#include "StateMachineFunctions.h"

#define ERROR 0.5 // Error en gramos

void setup()
{
    delay(500);
    Serial.begin(9600);
    Serial.println("Starting...");
    Serial.println("------------------------");
    // boton 2
    pinMode(messageToggleButtonPin, INPUT_PULLUP);
    // Configuración del botón con interrupción
    pinMode(resetButtonPin, INPUT_PULLUP);
    // Inicializar balanzas
    scaleCoffee.begin(scaleCoffee_DOUT_PIN, scaleCoffee_SCK_PIN);
    scaleCoffee.set_offset(scaleCoffee_OFFSET);
    scaleCoffee.set_scale(scaleCoffee_FACTOR);
    scaleCoffee.tare(20);
    // Set mode of real time running average to 7 and filter coefficient to 0.5 for
    scaleCoffee.set_runavg_mode();

    scaleChemex.begin(scaleChemex_DOUT_PIN, scaleChemex_SCK_PIN);
    scaleChemex.set_offset(scaleChemex_OFFSET);
    scaleChemex.set_scale(scaleChemex_FACTOR);
    scaleChemex.tare(20);
    scaleChemex.set_medavg_mode();
    // señales
    pinMode(weigth_DIN_PIN, OUTPUT);
    pinMode(dispenser_enabled, OUTPUT);
    pinMode(tare_DOUT_PIN, INPUT_PULLUP);
    pinMode(enable_DOUT_PIN, INPUT_PULLUP);
    pinMode(resetButtonPin, INPUT_PULLUP); // Configura el botón como entrada
    // inicializar lcd
    lcd.init();      // Inicializa el LCD
    lcd.backlight(); // Enciende la retroiluminación del LCD
    lcd.setCursor(0, 0);
    // Servo del mecanismo
    mechanicalServo.attach(pinServo);
    // Set angle to 0
    mechanicalServo.write(0);
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
        cafe_molido = cafe_molido + mappedValue;
        lcd.print(cafe_molido);
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
        scaleChemex_grams = scaleChemex.get_units(7); // la balanza del chemex solo medira
        Serial.print(scaleChemex_grams, 1);
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
        if (chemex - scaleChemex_grams <= ERROR)
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
        digitalWrite(dispenser_enabled, HIGH);
        digitalWrite(weigth_DIN_PIN, LOW);
        Serial.print("State 3: Weighing Coffee\nPeso: ");
        scaleCoffee_grams = scaleCoffee.get_units(7); // ahora la balanza del cafe sera la que mida
        Serial.print(scaleCoffee_grams, 1);
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
        grThreshold = cafe_molido - 10;
        if (scaleCoffee_grams < grThreshold && !flagServo)
        {
            mechanicalServo.write(maxAngle); // In this case only the servo will be activated once
            flagServo = true;
        }
        else if (scaleCoffee_grams >= grThreshold)
        {
            servoAngle = maxAngle - 22.5 * (scaleCoffee_grams - (cafe_molido - 2));
            mechanicalServo.write(servoAngle);
            flagServo = false; // Resetear la bandera
        }
        if (cafe_molido - scaleCoffee_grams <= ERROR)
        {
            mechanicalServo.write(0);
            Serial.println("Coffee reached the desired weight");
            digitalWrite(weigth_DIN_PIN, HIGH);
            Serial.println("\nWaiting for the signal tare to jump to the next state.");
            int tare_digital = digitalRead(tare_DOUT_PIN);
            if (tare_digital == 1)
            {
                tare_function();
            }
        }
    }
    break;
    case 4:
    {
        digitalWrite(weigth_DIN_PIN, LOW);
        Serial.print("State 4: Weighing Water 1\nPeso: ");
        scaleChemex_grams = scaleChemex.get_units(7);
        Serial.print(scaleChemex_grams, 1);
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
        if (agua_1 - scaleChemex_grams <= ERROR)
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
        scaleChemex_grams = scaleChemex.get_units();
        Serial.print(scaleChemex_grams, 1);
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
        if (agua_2 - scaleChemex_grams <= ERROR)
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