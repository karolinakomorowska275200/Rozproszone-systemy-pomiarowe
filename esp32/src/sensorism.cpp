#include <Arduino.h>
#include <math.h>

float sinsim(float avr, float ampl, float peroid){ // period w [s]
    float arg = 2 * M_PI * millis() * 0.001 / peroid;
    return avr + ampl * sin(arg);
}

float getsimAzimuth(){

    return sinsim(180.0, 10.0, 60.0); 
}

