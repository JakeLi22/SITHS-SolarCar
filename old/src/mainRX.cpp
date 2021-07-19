#include "main.hpp"
/*
    This is the main code loop for the transmission of code

    void setup():
        in charge of initializing the serial rates for the arduino transmitter and the lora (long range) radio transmitter
        Additionally, it sets the pin defined by "pin" to mode output


    void loop():
        if the long range transmitter is "available" set pin to high, print the lora data to serial, then set pin low
*/

LoraSerial lora(8, 9);
int pin = 13;

void setup()
{
    Serial.begin(9600);
    lora.begin(9600);
    pinMode(pin, OUTPUT);
}

void loop()
{
    if (lora.available())
    {
        digitalWrite(pin, HIGH);
        Serial.println(lora.readString());
        digitalWrite(pin, LOW);
    }
}
