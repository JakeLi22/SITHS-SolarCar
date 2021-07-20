#pragma once

/*
    This is the main code loop for the transmission of code

    void setup():
        in charge of initializing the serial rates for the arduino transmitter and the lora (long range) radio transmitter
        Additionally, it sets the pin defined by "pin" to mode output


    void loop():
        if the long range transmitter is "available" set pin to high, print the lora data to serial, then set pin low
*/
#include <math.h>
#include <Arduino.h>
#include <LoraSerial.hpp>
#include <GPSSerial.hpp>
#include <LCDObject.hpp>
#include <ADCObject.hpp>

// recieve pin from radio
const int RECEIVE_PIN = 8;
// transmit pin from radio
const int TRANSMIT_PIN = 9;
// baud rate
const int BAUD_RATE = 9600;
// number of bytes in the input message
const int MESSAGE_LENGTH = 4

    // 0-256 bat voltage            8 bits
    // 0-16 array current           4 bits
    // 0-256 motor current          8 bits
    // 0-16 aux battery voltage     4 bits
    // 0-256 speed                  8 bits

    // total bits                   32 bits
    // total bytes                  4

    //Initialize LoRa(receive pin, transmit pin)
    LoraSerial lora(RECEIVE_PIN, TRANSMIT_PIN);

void setup()
{
    // Initialize arduino output serial rate
    Serial.begin(BAUD_RATE);
    // Initialize lora serial rate
    lora.begin(BAUD_RATE);
}

int *get_next_message(LoraSerial &lora, const int message_length)
{
    if (message_length > lora.available())
    {
        return nullptr;
    }

    int *output = new int[message_length];
    for (int i = 0; i < message_length; i++)
    {
        output[i] = lora.read()
    }

    return output;
}

void loop()
{
    // This is the main logical loop, and will run until terminated

    // if the lora buffer has information in it
    // wait until we have a full message, and then print
    // that to serial. If we do not have a full message,
    // wait for one

    // this gives us our buffer size
    int buffer_size = lora.available();
    // tells us if the buffer has at least one full word
    bool ready;
    int *message = new int[MESSAGE_LENGTH];
    // if the buffer has at least one message in it
    if (buffer_size > MESSAGE_LENGTH)
    {

        // for each full message in the buffer
        for (int i = 0; i < floor(buffer_size / MESSAGE_LENGTH); i++)
        {
            message = get_message(lora);
        }
    }
    int num_words_in_buffer;
}