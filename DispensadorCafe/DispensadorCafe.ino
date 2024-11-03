#include <Servo.h>

// Pin enable, received from the state machine
int interruptPin = 2;

// Servo to open and close the mechanism
Servo servo1;
const int pinServo1 = 7; // Pin 9 doesn't work

// TB6612FNG Motor Driver used to control the motor that shakes the coffee
// Motor A
int PWMA = 3; // speed control
int AIN1 = 10;
int AIN2 = 11;

// Potentiometer to control the speed of the motor
int potentiometerPin = A2;

// Control de apertura del mecanismo
int mechanismControlPin = A4;

// Variables to store the last values
int lastPotentiometerValue = 0;
int lastMechanismControlValue = 0;

void setup()
{
    servo1.attach(pinServo1);
    pinMode(PWMA, OUTPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(interruptPin, INPUT);

    // Initialize the serial port
    Serial.begin(9600);
    // Shake the coffee motor stop
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    Serial.println("Starting...");
}

void loop()
{
    // Read the potentiometer value
    int currentPotentiometerValue = analogRead(potentiometerPin);
    // Check if the difference is greater or equal to 5 for the potentiometer
    if (abs(currentPotentiometerValue - lastPotentiometerValue) >= 5)
    {
        // Map the potentiometer value to a value between 0 and 255
        int speed = map(currentPotentiometerValue, 0, 1023, 0, 255);
        // Move the motor and change the speed
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        analogWrite(PWMA, speed);
        Serial.print("Speed: ");
        Serial.println(speed);
        // Update the last potentiometer value
        lastPotentiometerValue = currentPotentiometerValue;
    }

    delay(500);
}