#include "main.hpp"

#define INTERVAL_SEC 15
#define V_REF 1.1
#define R1 20
#define R2 1
// this is inverse   should be r2 \/ r1 + r2
#define VDIV_RATIO (R1 + R2) / R2

// Initialize the GPS serial
GPSSerial gpsSerial;
// Initialize the lora serial
LoraSerial lora(2, 3);
// Initialize an lcd object (whatever that is)
LCDObject lcd(0x27, 20, 4);
// ADCObject adc;

// I wonder what this is for
unsigned long _lastSend = 0;

// for some reason declaring the descriptively names mainFunc
// before you define it. If you are using a header file then this should go there
int mainFunc(bool gpsEnabled);

// More void setups. You should consider making two folders in two projects for RX and TX which I assume are recieve and transmit

void setup()
{
    // set baud rates
    // initiate serial coms
    Serial.begin(9600);
    lora.begin(9600);
    gpsSerial.begin(9600);
    // initialize lcd
    // what is an lcd???  A monitor?
    lcd.start(true);
    // set reference voltage to internal 1.1v
    analogReference(INTERNAL);
    // why analog read a0
    analogRead(A0);
}

void loop()
{

    // whats the point of having GPS (jake has told me speed)
    // you should definately consider breaking this up
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
    // vAux = auxiliary voltage
    /*
        so how you do this math
        you have a voltage dividor which gives you your vdiv ratio

        you're reading anything between 0 and 1024 
        you need a number

        // you should use a pin defined on the top
        // voltage to analog input should not exceed analog reference
        // you also should keep the internal reference to the default 5V 
        vAux = analogRead(A0) * V_REF/1024.0 * VDIV_RATIO
    */
    double vAux = (double)analogRead(A0) * (V_REF / 1023.0) * VDIV_RATIO;
    // Serial.println(vAux);

    time_t currentTime;
    int speed;

    if (gpsEnabled)
    {
        // if gps is locking, pass and indicate
        // if gps is locking this isnt a pass, this is a kill
        // you shouldnt return 1 for all of your functions no matter what happens, create codes in an enum
        bool locking = !gpsSerial.gpsLocked();
        lcd.indCondPrint("LOCKING", 0, locking);
        if (locking)
            return 1;

        // if gps collecting on same second, pass
        // same comment as before
        if (!gpsSerial.secondUpdated())
            return 1;

        // get data from GPS
        // why arent you returning this stuff?
        // nvm, I see
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