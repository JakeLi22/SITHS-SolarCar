#pragma once

#include <Arduino.h>
#include <AltSoftSerial.h>
#include <TinyGPS++.h>
#include <TimeLib.h>

class GPSSerial : public AltSoftSerial
{
private:
    time_t _secondUpdate = 0;
    time_t _intervalUpdate = 0;
public:
    using AltSoftSerial::AltSoftSerial;

    // initiate gps object
    TinyGPSPlus gps;

    // encode incoming data from gps
    bool GPSencode();

    // get unix time from GPS
    time_t getUnixTime();

    // return if the gps is encoding new second
    bool secondUpdated();

    // return if the time has reached at given interval
    bool intervalUpdated(int interval);

    // get rounded speed in integer MPH
    int getSpeed();

    // check if gps has lock
    bool gpsLocked();

};