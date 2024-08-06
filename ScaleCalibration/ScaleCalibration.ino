#include <Arduino.h> // Required for platformio, not needed for Arduino IDE
#include <Wire.h> // Required when using I2C devices
#include "HX711.h" // Required for HX711 load cell amplifier

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;

HX711 loadCell;

void setup() {
    Serial.begin(9600);
    Serial.println("HX711 calibration sketch");
    Serial.println("------------------------");
    loadCell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void loop(){
    calibrate();
}

void calibrate()
{
    // Print initial messages to indicate the start of the calibration process
    Serial.println("\n\nCALIBRATION\n===========");
    Serial.println("remove all weight from the loadcell");
    
    // Clear the serial input buffer
    while (Serial.available()) Serial.read();

    // Ask the user to press Enter after removing all weight
    Serial.println("and press enter\n");
    while (Serial.available() == 0);

    // Determine the offset when there is no weight on the load cell
    Serial.println("Determine zero weight offset");
    loadCell.tare(20);  // Average 20 measurements
    uint32_t offset = loadCell.get_offset();

    // Print the determined offset value
    Serial.print("OFFSET: ");
    Serial.println(offset);
    Serial.println();

    // Ask the user to place a known weight on the load cell
    Serial.println("place a weight on the loadcell");
    
    // Clear the serial input buffer
    while (Serial.available()) Serial.read();

    // Ask the user to enter the weight in grams and press Enter
    Serial.println("enter the weight in (whole) grams and press enter");
    uint32_t weight = 0;
    
    // Read the weight entered by the user from the serial monitor
    while (Serial.peek() != '\n')
    {
        if (Serial.available())
        {
            char ch = Serial.read();
            if (isdigit(ch))
            {
                weight *= 10;
                weight = weight + (ch - '0');
            }
        }
    }
    
    // Print the weight entered by the user
    Serial.print("WEIGHT: ");
    Serial.println(weight);
    
    // Calibrate the load cell using the known weight and averaging 20 measurements
    loadCell.calibrate_scale(weight, 20);
    float scale = loadCell.get_scale();

    // Print the determined scale factor
    Serial.print("SCALE:  ");
    Serial.println(scale, 6);

    // Provide instructions to the user on how to use the offset and scale values in the setup of their project
    Serial.print("\nuse scale.set_offset(");
    Serial.print(offset);
    Serial.print("); and scale.set_scale(");
    Serial.print(scale, 6);
    Serial.print(");\n");
    Serial.println("in the setup of your project");

    // Print a final message indicating the end of the calibration process
    Serial.println("\n\n");
}