#include <Servo.h>
//Pin enable, recived from the state machine
int interruptPin = 2;  
// Crear instancias de servomotores
Servo servo1;

// Definir los pines a los que están conectados los servos
const int pinServo1 = 5;//Pin 9 dosn't work

// TB6612FNG Motor Driver used to control the motor that shakes the coffee 
// Motor A
int PWMA = 3; // control de vel
int AIN1 = 10; // sentido
int AIN2 = 11; // sentido

// Potentiometer to control the speed of the motor
int potentiometer = A2;

void setup() {
    // Adjuntar los servos a los pines correspondientes
    servo1.attach(pinServo1);
    pinMode(PWMA, OUTPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(interruptPin, INPUT);

    // Inicializar la comunicación serial
    Serial.begin(9600);
    //shake the coffee motor stop
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
}

void loop() {
    // Leer el valor del potenciómetro y mapearlo a una velocidad
    if(digitalRead(interruptPin) == HIGH){
        Serial.println("Activating system");
        //First open mechanism
        servo1.write(60);
        delay(2000);
        //Then shake the coffee
        while (digitalRead(interruptPin) == HIGH){
            int speed = analogRead(potentiometer);
            speed = map(speed, 0, 1023, 0, 255); // map the potentiometer value to a value between 0 and 255
            // Print the speed to the serial port
            Serial.print("Speed: ");
            Serial.println(speed);
            // Move the motor and change the speed
            digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, HIGH);
            analogWrite(PWMA, speed);
        }
        //Close mechanism
        servo1.write(0);
        delay(2000);
    }
}