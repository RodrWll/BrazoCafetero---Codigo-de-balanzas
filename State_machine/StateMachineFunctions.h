// StateMachineFunctions.h
#ifndef STATE_MACHINE_FUNCTIONS_H
#define STATE_MACHINE_FUNCTIONS_H

// Constants for scale coffee
const int scaleCoffee_DOUT_PIN = 8;
const int scaleCoffee_SCK_PIN = 9;
const double scaleCoffee_FACTOR = 420.52;
const int scaleCoffee_OFFSET = 4294906620;
// Constants for scale chemex
const int scaleChemex_DOUT_PIN = 4;
const int scaleChemex_SCK_PIN = 5;
const double scaleChemex_FACTOR = -447.52;
const int scaleChemex_OFFSET = 617864;
// Constants for inputs - xArm
const int tare_DOUT_PIN = 3;
const int enable_DOUT_PIN = 7;
// Constants for outputs - xArm
const int weigth_DIN_PIN = 12;
// Constants for outputs - dispenser
const int dispenser_enabled = 6;
// Constants for other inputs
const int resetButtonPin = 2;
const int messageToggleButtonPin = 11;

// Constants for potentiometer
const int potPin = A1;
const int pinServo = 10;

// Internal variables
double chemex = 400;
double cafe_molido = 20;
double agua_1 = 20;
double agua_2 = 20;
int state = 1;
double scaleCoffee_grams = 0;
double scaleChemex_grams = 0;
int resetButtonState = 0;
int lastResetButtonValue = 0;
unsigned long lastTimeButtonStateChanged = 0;
unsigned long lastResetButtonPressTime = 0;
int lastButtonValue = 0;
int potValue = 0;
int lastPotValue = 0;
int mappedValue = 0;

// Servo mechanism
const int maxAngle = 65;

// start closing angle
int angleClosing = 30;
int grStartClosing = 15;
int grEndClosing = 5;
int grindThresholdStart = cafe_molido - grStartClosing;
int grindThresholdEnd = cafe_molido - grEndClosing;
float slope = (float)(maxAngle - angleClosing) / (grStartClosing - grEndClosing);
float servoAngle = 0;
bool flagServo = false;
int mensaje = 0;

// Objects
LiquidCrystal_I2C lcd(0x27, 16, 2);
// HX711 circuit wiring
HX711 scaleCoffee;
HX711 scaleChemex;
Servo mechanicalServo;

// Function to tare the scales
int tare_function()
{
    scaleCoffee.set_offset(scaleCoffee_OFFSET);
    scaleCoffee.tare(20);
    scaleChemex.set_offset(scaleChemex_OFFSET);
    scaleChemex.tare(20);
    return state = state + 1;
}
// Function to show the coffee message
void coffeeMessage()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("COFFEE ");
    lcd.print(" Gr:");
    lcd.print(scaleCoffee_grams);
    lcd.setCursor(0, 1);
    lcd.print("St:");
    lcd.print(state);
    lcd.print("|En: ");
    lcd.print(digitalRead(enable_DOUT_PIN));
    lcd.print("|Ta: ");
    lcd.print(digitalRead(tare_DOUT_PIN));
}
// Function to show the chemex message
void chemexMessage()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CHEMEX ");
    lcd.print(" Gr:");
    lcd.print(scaleChemex_grams);
    lcd.setCursor(0, 1);
    lcd.print("St:");
    lcd.print(state);
    lcd.print("|En: ");
    lcd.print(digitalRead(enable_DOUT_PIN));
    lcd.print("|Ta: ");
    lcd.print(digitalRead(tare_DOUT_PIN));
}
// Function to reset the states
void resetButtonAction()
{
    if (millis() - lastResetButtonPressTime > DEBOUNCE)
    {
        int resetButtonState = digitalRead(resetButtonPin);
        if (resetButtonState != lastResetButtonValue)
        {
            lastResetButtonPressTime = millis();
            lastResetButtonValue = resetButtonState;
            if (resetButtonState == HIGH)
            {
                state = 1;
                scaleCoffee_grams = 0;
                scaleChemex_grams = 0;
                scaleCoffee.tare(20);
                scaleChemex.tare(20);
                mechanicalServo.write(0);
                flagServo = false;
            }
        }
    }
}
// Function to read the button
void toggleScreenButtonAction()
{
    if (millis() - lastTimeButtonStateChanged > DEBOUNCE)
    {
        int buttonState = digitalRead(messageToggleButtonPin);
        if (buttonState != lastButtonValue)
        {
            lastTimeButtonStateChanged = millis();
            lastButtonValue = buttonState;
            if (buttonState == HIGH)
            {
                if (mensaje == 1)
                {
                    mensaje = 2;
                }
                else
                {
                    mensaje = 1;
                }
            }
        }
    }
}

#endif // STATE_MACHINE_FUNCTIONS_H