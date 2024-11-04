#include <Wire.h>  // Required when using I2C devices
#include "HX711.h" // Required for HX711
HX711 scaleCoffee;
HX711 scaleChemex;
// Constants for scale coffee
const int scaleCoffee_DOUT_PIN = 8;
const int scaleCoffee_SCK_PIN = 9;
const double scaleCoffee_FACTOR = 420.52;
const int scaleCoffee_OFFSET = 4294906620;
// Constants for scale chemex
const int scaleChemex_DOUT_PIN = 4;
const int scaleChemex_SCK_PIN = 5;
const double scaleChemex_FACTOR = -430.52;
const int scaleChemex_OFFSET = 617864;

void setup()
{
    Serial.begin(9600);
    Serial.println("Starting...");
    Serial.println("------------------------");
    // Inicializar balanzas
    scaleCoffee.begin(scaleCoffee_DOUT_PIN, scaleCoffee_SCK_PIN);
    scaleCoffee.set_offset(scaleCoffee_OFFSET);
    scaleCoffee.set_scale(scaleCoffee_FACTOR);
    // scaleCoffee.tare(20);//This overwrites the offset

    scaleChemex.begin(scaleChemex_DOUT_PIN, scaleChemex_SCK_PIN);
    scaleChemex.set_offset(scaleChemex_OFFSET);
    scaleChemex.set_scale(scaleChemex_FACTOR);
}
int times = 10;
// get_units() returns the weight in grams
//  get_units = get_value() * _scale - _offset

void loop()
{
    if (Serial.available() > 0)
    {
        char option = Serial.read();
        if (option == '2')
        {
            Serial.print("Chemex: ");
            // Normal read
            Serial.println(scaleChemex.get_units(10));
            // Running average
            scaleChemex.set_runavg_mode();
            Serial.print("Chemex Running Average: ");
            Serial.println(scaleChemex.get_units(10));
            // Median
            scaleChemex.set_median_mode();
            Serial.print("Chemex Median: ");
            Serial.println(scaleChemex.get_units(10));
            // Median average
            scaleChemex.set_medavg_mode();
            Serial.print("Chemex Median Average: ");
            Serial.println(scaleChemex.get_units(10));
            // Average
            scaleChemex.set_average_mode();
            Serial.print("Chemex Average: ");
            Serial.println(scaleChemex.get_units(10));
        }
        else
        {
            Serial.print("Coffee: ");
            // Normal read
            Serial.println(scaleCoffee.get_units(10));
            // Running average
            scaleCoffee.set_runavg_mode();
            Serial.print("Coffee Running Average: ");
            Serial.println(scaleCoffee.get_units(10));
            // Median
            scaleCoffee.set_median_mode();
            Serial.print("Coffee Median: ");
            Serial.println(scaleCoffee.get_units(10));
            // Median average
            scaleCoffee.set_medavg_mode();
            Serial.print("Coffee Median Average: ");
            Serial.println(scaleCoffee.get_units(10));
            // Average
            scaleCoffee.set_average_mode();
            Serial.print("Coffee Average: ");
            Serial.println(scaleCoffee.get_units(10));
        }
        Serial.println("------------------------");
    }
    delay(1000);
}
