#include <Servo.h>

//Pin enable, recived from the state machine
int interruptPin = 2;  

//Servo to open and close the mechanism
Servo servo1;
const int pinServo1 = 7; //Pin 9 dosn't work

// TB6612FNG Motor Driver used to control the motor that shakes the coffee 
// Motor A
int PWMA = 3; // speed control
int AIN1 = 10; 
int AIN2 = 11; 

// Potentiometer to control the speed of the motor
int potentiometer = A2;

// Control de apertura del mecanismo
int potenMecha = A4;

// Variables for moving average filter
const int numReadings = 5;
int readingsPotentiometer[numReadings]; // Array to store potentiometer readings
int readingsPotenMecha[numReadings]; // Array to store potenMecha readings
int readIndexPotentiometer = 0; // Index of the current reading
int readIndexPotenMecha = 0; // Index of the current reading
int totalPotentiometer = 0; // Sum of the readings
int totalPotenMecha = 0; // Sum of the readings
int averagePotentiometer = 0; // Average of the readings
int averagePotenMecha = 0; // Average of the readings

void setup() {
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

    // Initialize all the readings to 0
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
        readingsPotentiometer[thisReading] = 0;
        readingsPotenMecha[thisReading] = 0;
    }
}

void loop() {
    //Serial.println("Activating system");

    // Read the potentiometer value
    totalPotentiometer = totalPotentiometer - readingsPotentiometer[readIndexPotentiometer];
    readingsPotentiometer[readIndexPotentiometer] = analogRead(potentiometer);
    totalPotentiometer = totalPotentiometer + readingsPotentiometer[readIndexPotentiometer];
    readIndexPotentiometer = (readIndexPotentiometer + 1) % numReadings;
    averagePotentiometer = totalPotentiometer / numReadings;

    // Read the potenMecha value
    totalPotenMecha = totalPotenMecha - readingsPotenMecha[readIndexPotenMecha];
    readingsPotenMecha[readIndexPotenMecha] = analogRead(potenMecha);
    totalPotenMecha = totalPotenMecha + readingsPotenMecha[readIndexPotenMecha];
    readIndexPotenMecha = (readIndexPotenMecha + 1) % numReadings;
    averagePotenMecha = totalPotenMecha / numReadings;

    // Map the potentiometer value to a value between 0 and 255
    int speed = map(averagePotentiometer, 0, 1023, 0, 255);
    // Map the angle of the mechanism
    averagePotenMecha = averagePotenMecha/8;
    int angle = map(averagePotenMecha, 0, 127, 0, 75);

    // Print the speed to the serial port
    // Serial.print("Speed: ");
    // Serial.println(speed);
    // Print the angle to the serial port
    // Serial.print("Angle: ");
    // Serial.println(angle);
    // Space to serial port
    // Serial.println("------------------------");

    // Move the motor and change the speed
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    analogWrite(PWMA, speed);
    servo1.write(angle);
  delay(500);
}