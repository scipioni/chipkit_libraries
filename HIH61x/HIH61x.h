#ifndef __HIH61x_H__
#define __HIH61x_H__

#define NOT_UPDATED 20

#include "WProgram.h"

class HIH61x {
public:
    HIH61x(int, unsigned long);
    void init(void);
    byte fetch(unsigned long);
    
    float temp;
    float hum;
    float hum_abs;
    byte status;
    int pin_active;
private:
    unsigned long time_fetch;
    unsigned long refresh;
};

#endif // __HIH61x_H__
