#include "main.hpp"

#define INTERVAL_SEC 15
#define V_REF 1.1
#define R1 20
#define R2 1
#define VDIV_RATIO (R1 + R2) / R2

GPSSerial gpsSerial;
LoraSerial lora(2, 3);
LCDObject lcd(0x27, 20, 4);
// ADCObject adc;

unsigned long _lastSend = 0;

int mainFunc(bool gpsEnabled);

void setup()
{
    // initiate serial coms
    Serial.begin(9600);
    lora.begin(9600);
    gpsSerial.begin(9600);
    // initialize lcd
    lcd.start(true);
    // set reference voltage to internal 1.1v
    analogReference(INTERNAL);
    analogRead(A0);
}

void loop()
{

    while (gpsSerial.available())
        if (gpsSerial.GPSencode())
            mainFunc(true);
    // if gps is missing / not encoding stuff
    //#TODO: maybe you want to have a timeout and then a retry instead of 
    // turining the entire system off until reset with a while true loop
    if (millis() > 5000 && gpsSerial.gps.charsProcessed() < 10)
        while (true)
            mainFunc(false);
}

int mainFunc(bool gpsEnabled)
{
    // measure aux voltage (arduino's own voltage)
    // if bandwidth is a concern you can get away with a 32 bit float
    double vAux = (double)analogRead(A0) * (V_REF / 1023.0) * VDIV_RATIO;
    // Serial.println(vAux);

    time_t currentTime;
    int speed;

    if (gpsEnabled)
    {
        // if gps is locking, pass and indicate
        bool locking = !gpsSerial.gpsLocked();
        lcd.indCondPrint("LOCKING", 0, locking);
        if (locking)
            return 1;

        // if gps collecting on same second, pass
        if (!gpsSerial.secondUpdated())
            return 1;

        // get data from GPS
        currentTime = gpsSerial.getUnixTime();
        speed = gpsSerial.getSpeed();
    }
    else
    {
        // indicate no GPS, nullify time and speed
        lcd.indCondPrint("NO GPS!", 0, true);
        currentTime = 0;
        speed = 0;
        // delay for 1 sec
        delay(1000);
    }

    // print to lcd
    lcd.displayData(currentTime, speed, 0, vAux, 0, 0);

    // send data via lora
    if (millis() > (_lastSend + (INTERVAL_SEC * 1000)))
    {
        lora.sendData(speed, 0, vAux, 0, 0);

        _lastSend = millis();
    }
    return 0;
}