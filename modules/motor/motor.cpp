//=====[Libraries]=============================================================

#include "mbed.h"

#include "arm_book_lib.h"

#include "motor.h"

//=====[Declaration of private defines]========================================

#define MOTOR_UPDATE_TIME 9

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalInOut motorM1Pin1(PF_2);
DigitalInOut motorM1Pin2(PE_3);

DigitalInOut motorM2Pin1();
DigitalInOut motorM2Pin2();



//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

motorDirection_t motorDirection;
motorDirection_t motorState;

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void motorControlInit()
{
    motorM1Pin1.mode(OpenDrain);
    motorM1Pin2.mode(OpenDrain);
    
    motorM1Pin1.input();
    motorM1Pin2.input();



    motorDirection = STOPPED;
    motorState = STOPPED;
}

motorDirection_t motorDirectionRead()
{
    return motorDirection;
}

void motorDirectionWrite( motorDirection_t direction )
{
    motorDirection = direction;
}

void motorControlUpdate()
{
    static int motorUpdateCounter = 0;
    
    motorUpdateCounter++;
    
    if ( motorUpdateCounter > MOTOR_UPDATE_TIME ) {
        
        motorUpdateCounter = 0;
        
        switch ( motorState ) {
            case DIRECTION_1:
                if ( motorDirection == DIRECTION_2 || 
                     motorDirection == STOPPED ) {
                    motorM1Pin1.input();
                    motorM1Pin2.input();
                    motorState = STOPPED;
                }
            break;
    
            case DIRECTION_2:
                if ( motorDirection == DIRECTION_1 || 
                     motorDirection == STOPPED ) {
                    motorM1Pin1.input();
                    motorM1Pin2.input();
                    motorState = STOPPED;
                }
            break;
    
            case STOPPED:
            default:
                if ( motorDirection == DIRECTION_1 ) {
                    motorM1Pin2.input();
                    motorM1Pin1.output();
                    motorM1Pin1 = LOW;
                    motorState = DIRECTION_1;
                }
                
                if ( motorDirection == DIRECTION_2 ) {
                    motorM1Pin1.input();
                    motorM1Pin2.output();
                    motorM1Pin2 = LOW;
                    motorState = DIRECTION_2;
                }
            break;
        }
    }        
}

//=====[Implementations of private functions]==================================
