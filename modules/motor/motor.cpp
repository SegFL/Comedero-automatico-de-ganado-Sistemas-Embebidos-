//=====[Libraries]=============================================================

#include "mbed.h"

#include "arm_book_lib.h"

#include "motor.h"

//=====[Declaration of private defines]========================================


//=====[Declaration of private data types]=====================================



//=====[Declaration and initialization of public global objects]===============


motor::motor(PinName pin1, PinName pin2)
      : motorDirection(STOPPED), motorState(STOPPED), motorPin1(PF_2),
        motorPin2(PE_3) {

    // Configurar los pines

    motorDirection = STOPPED;
    motorState = STOPPED;

    DigitalInOut motorPin1(pin1);
    DigitalInOut motorPin2(pin2);

    motorPin1.mode(OpenDrain);
    motorPin2.mode(OpenDrain);

    motorPin1.input();
    motorPin2.input();
  }

motorDirection_t motor::read() { return motorDirection; }

void motor::write(motorDirection_t d) { motorDirection = d; }

motorDirection_t motor::read_state() { return motorState; }

void motor::change_state(motorDirection_t state) {

    if (state == STOPPED) {
      motorPin1.input();
      motorPin2.input();
      motorState = state;
    }

    if (motorDirection == DIRECTION_1) {
      motorPin2.input();
      motorPin1.output();
      motorPin1 = LOW;
      motorState = DIRECTION_1;
    }

    if (motorDirection == DIRECTION_2) {
      motorPin1.input();
      motorPin2.output();
      motorPin2 = LOW;
      motorState = DIRECTION_2;
    }
}


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

//=====[Implementations of private functions]==================================
