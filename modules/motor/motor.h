//=====[#include guards - begin]===============================================

#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "mbed.h"
//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

typedef enum {
    DIRECTION_1,
    DIRECTION_2,
    STOPPED
} motorDirection_t;

class motor{
    public:
        motorDirection_t motorDirection;
        motorDirection_t motorState;
        DigitalInOut motorPin1;
        DigitalInOut motorPin2;
        
        motor(PinName pin1, PinName pin2);
        motorDirection_t read_state();
        void change_state(motorDirection_t state);
        motorDirection_t read();
        void write(motorDirection_t d);
 };


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _MOTOR_H_
