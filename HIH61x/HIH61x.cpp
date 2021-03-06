/*
 * Thaks to Peter H Anderson, Baltimore for HIH6130 example
 *
 *
 */

#include "WProgram.h"
#include "HIH61x.h"
#include <Wire.h> //I2C library
#include <math.h>

//#define DEBUG 

#ifdef DEBUG
  #define DEBUG_PRINT(x)     Serial.print (x)
  #define DEBUG_PRINTDEC(x)     Serial.print (x, DEC)
  #define DEBUG_PRINTLN(x)  Serial.println (x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTDEC(x)
  #define DEBUG_PRINTLN(x)
#endif 

HIH61x::HIH61x(int _pin_active, unsigned long _refresh) {
    temp=0;
    hum=0;
    hum_abs=0;
    status=0;
    pin_active = _pin_active;  
    time_fetch=0;
    refresh = _refresh;
}

void HIH61x::init(void) {
    pinMode(pin_active, OUTPUT);
    digitalWrite(pin_active, HIGH); // power off device
}

/*
 *
      switch(_status)
      {
          case 0:  Serial.println("Normal.");
                   break;
          case 1:  Serial.println("Stale Data.");
                   break;
          case 2:  Serial.println("In command mode.");
                   break;
          default: Serial.println("Diagnostic."); 
                   break; 
      }    
*/

byte HIH61x::fetch(unsigned long now) {
    byte address, Hum_H, Hum_L, Temp_H, Temp_L;
    unsigned int H_dat, T_dat;
    float pwS;
    address = 0x27;

    if ((now - time_fetch) < refresh)
        return NOT_UPDATED;

    time_fetch = now;

    digitalWrite(pin_active, LOW); // power on sensor
    DEBUG_PRINT("setting low pin ");
    DEBUG_PRINTLN(pin_active);
    delay(100); // wait until device is active


    Wire.beginTransmission(address); 
    Wire.endTransmission();
    delay(100);

    Wire.requestFrom((int)address, (int) 4);
    if(Wire.available() < 4)
        return NOT_UPDATED;
    
    Hum_H = Wire.receive();
    Hum_L = Wire.receive();
    Temp_H = Wire.receive();
    Temp_L = Wire.receive();

    status = (Hum_H >> 6) & 0x03;
    Hum_H = Hum_H & 0x3f;
    H_dat = (((unsigned int)Hum_H) << 8) | Hum_L;
    T_dat = (((unsigned int)Temp_H) << 8) | Temp_L;
    T_dat = T_dat / 4;
    hum = (float) H_dat * 6.10e-3;
    temp = (float) T_dat * 1.007e-2 - 40.0;
    DEBUG_PRINT("temp:");
    DEBUG_PRINTLN(temp);

    /* absolute humidity g/m3 */
    pwS=6.0964*powf(10.0, temp*7.33365/(temp+230.5));
    DEBUG_PRINT("pws:");
    DEBUG_PRINTLN(pwS);
    hum_abs = 2.16679*pwS*hum/(100.0*(273.16+temp));
    DEBUG_PRINT("hum_abs:");
    DEBUG_PRINTLN(hum_abs);
    
    digitalWrite(pin_active, HIGH); // power off sensor
    return status;
}

