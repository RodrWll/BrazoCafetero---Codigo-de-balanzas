#include <Arduino.h> // Required for platformio, not needed for Arduino IDE
#include <Wire.h>    // Required when using I2C devices
#include "HX711.h"   // Required for HX711 load cell amplifier
#include <LiquidCrystal_I2C.h>
#include "Servo.h"
#define ERROR 0.5 // Error en gramos
#define DEBOUNCE 50
#include "StateMachineFunctions.h"

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
    scaleCoffee.set_scale(scaleCoffee_FACTOR);
    scaleCoffee.tare(20);
    // Set mode of real time running average to 7 and filter coefficient to 0.5 for
    scaleCoffee.set_runavg_mode();

    scaleChemex.begin(scaleChemex_DOUT_PIN, scaleChemex_SCK_PIN);
    scaleChemex.set_scale(scaleChemex_FACTOR);
    scaleChemex.tare(20);
    scaleChemex.set_runavg_mode();
    // señales
    pinMode(weigth_DIN_PIN, OUTPUT);
    pinMode(9, OUTPUT);
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
unsigned long previousMillis = 0; // Variable to store the last time coffeeMessage was called
const long interval = 200;        // Interval at which to call coffeeMessage (200 milliseconds)

int velDispensador = 125;

void loop()
{

    unsigned long currentMillis = millis();

    resetButtonAction();
    // toggleScreenButtonAction();

    // POTENCIOMETRO
    // potValue = analogRead(potPin);
    // mappedValue = map(potValue, 0, 1023, 0, 50);
    // if (abs(lastPotValue - mappedValue) > 5) // Change the value if the difference is greater than 2
    // {
    //     lcd.clear();
    //     lcd.setCursor(0, 0);
    //     lcd.print("Valor cafe: ");
    //     cafe_molido = cafe_molido + mappedValue;
    //     lcd.print(cafe_molido);
    //     lcd.setCursor(0, 1);
    //     delay(100);
    // }
    // lastPotValue = mappedValue;
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
            digitalWrite(weigth_DIN_PIN, LOW);
            tare_function();
        }
    }
    break;
    case 2:
    {
      
        Serial.print("State 2: Weighing Chemex\nPeso: ");
        scaleChemex_grams = scaleChemex.get_units(10); // la balanza del chemex solo medira
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
                digitalWrite(weigth_DIN_PIN, LOW);
                tare_function();
            }
        }
    }
    break;
    case 3:
    {
        // digitalWrite(dispenser_enabled, HIGH);
 
        Serial.print("State 3: Weighing Coffee\nPeso: ");
        scaleCoffee_grams = scaleCoffee.get_units(10); // ahora la balanza del cafe sera la que mida
        Serial.print(scaleCoffee_grams, 1);
        Serial.println(" gr");
        if (currentMillis - previousMillis >= interval)
        {
            previousMillis = currentMillis;
            coffeeMessage();
        }
        if (mensaje == 1)
        {
            chemexMessage();
        }
        else if (mensaje == 2)
        {
            coffeeMessage();
        }
        if (scaleCoffee_grams < grindThresholdStart /*&& !flagServo*/)
        {
            mechanicalServo.write(maxAngle); // In this case only the servo will be activated once
            analogWrite(dispenser_enabled, 128);
            flagServo = true;
        }
        // also cafe_molido - scaleCoffee_grams <= grStartClosing
        else if (grindThresholdStart <= scaleCoffee_grams && scaleCoffee_grams <= grindThresholdEnd)
        {
            // The servo will be activated to close the dispenser starts at 10 grams (grStartClosing) before the desired weight.
            // The angle will be closing as the coffee is being dispensed to 2 grams (grEndClosing) before the desired weight, and the final angle is defined by angleClosing now set to 25 degrees
            servoAngle = maxAngle - slope * (scaleCoffee_grams - (grindThresholdStart));
            mechanicalServo.write(servoAngle);
            analogWrite(dispenser_enabled, velDispensador * 1.3);
        }
        else if (cafe_molido - scaleCoffee_grams <= grEndClosing && cafe_molido - scaleCoffee_grams > ERROR)
        {
            // enable the dispenser PWM signal will be sent to the dispenser to reach the desired weight
            analogWrite(dispenser_enabled, velDispensador * 1.75);
        }

        if (cafe_molido - scaleCoffee_grams <= ERROR)
        {
            mechanicalServo.write(0);
            analogWrite(dispenser_enabled, 0);
            Serial.println("Coffee reached the desired weight");
            digitalWrite(weigth_DIN_PIN, HIGH);
            Serial.println("\nWaiting for the signal tare to jump to the next state.");
            int tare_digital = digitalRead(tare_DOUT_PIN);
            if (tare_digital == 1)
            {
                digitalWrite(weigth_DIN_PIN, LOW);
                // tare_function();
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
                    digitalWrite(weigth_DIN_PIN, LOW);
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
                    digitalWrite(weigth_DIN_PIN, LOW);
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
