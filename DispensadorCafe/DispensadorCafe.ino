#include <Servo.h>
//Pin enable, recived from the state machine
int interruptPin = 2;  

//Servo to open and close the mechanism
Servo servo1;
const int pinServo1 = 5;//Pin 9 dosn't work

// TB6612FNG Motor Driver used to control the motor that shakes the coffee 
// Motor A
int PWMA = 3; // speed control
int AIN1 = 10; int AIN2 = 11; 

// Potentiometer to control the speed of the motor
int potentiometer = A2;

//Control de apertura del mecanismo
int potenMecha = A4;

void setup() {
    servo1.attach(pinServo1);
    pinMode(PWMA, OUTPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(interruptPin, INPUT);

    // Initialize the serial port
    Serial.begin(9600);
    //shake the coffee motor stop
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
}

void loop() {
    //if(digitalRead(interruptPin) == HIGH){
        Serial.println("Activating system");
        //First open mechanism
        /*servo1.write(60);
        delay(2000);*/
        //Then shake the coffee

        //while (digitalRead(interruptPin) == HIGH){
            int speed = analogRead(potentiometer);
            int angle = analogRead(potenMecha);
            // map the potentiometer value to a value between 0 and 255
            speed = map(speed, 0, 1023, 0, 255);
            // map the angle of the mechanisim
            angle = map(angle,0,1023,0,75);
            // Print the speed to the serial port
            Serial.print("Speed: ");
            Serial.println(speed);
            // Print the angle to the serial port
            Serial.print("Angle: ");
            Serial.println(angle);
            // Space to serial port
            Serial.println("------------------------");
            // Move the motor and change the speed
            digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, HIGH);
            analogWrite(PWMA, speed);
            servo1.write(angle);
            delay(500);
        /*}
        //Close mechanism
        servo1.write(0);
        delay(2000);
    }*/
}
