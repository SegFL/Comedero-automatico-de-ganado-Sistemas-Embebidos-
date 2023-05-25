//=====[Libraries]=============================================================

#include "mbed.h"

#include "arm_book_lib.h"

#include "motor.h"

//=====[Declaration of private defines]========================================

#define MOTOR_UPDATE_TIME 9

//=====[Declaration of private data types]=====================================

class motor {
public:
  motorDirection_t motorDirection;
  motorDirection_t motorState;
  DigitalInOut motorPin1;
  DigitalInOut motorPin2;

  // Constructor
  motor(PinName pin1, PinName pin2)
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
  motorDirection_t read() { return motorDirection; }
  void write(motorDirection_t d) { motorDirection = d; }

  motorDirection_t read_state() { return motorState; }
  void change_state(motorDirection_t state) {

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
};

//=====[Declaration and initialization of public global objects]===============

motor motorArray[] = {motor(PF_2, PE_3), motor(PH_0, PH_1)};

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void motorControlInit() {}

motorDirection_t motorDirectionRead(int n_motor) {
  return motorArray[n_motor].read();
}

void motorDirectionWrite(int n_motor, motorDirection_t direction) {

  motorArray[n_motor].write(direction);
}

void motorControlUpdate() {
  // Ambos motores tienen el mismo counter lo que singifica que ambos se
  // actualizan en el mismo ciclo.
  static int motorUpdateCounter = 0;

  motorUpdateCounter++;

  if (motorUpdateCounter > MOTOR_UPDATE_TIME) {

    motorUpdateCounter = 0;
    for (int i = 0; i < 2; i++) {

      switch (motorArray[i].read_state()) {
      case DIRECTION_1:
        if (motorArray[i].read() == DIRECTION_2 ||
            motorArray[i].read() == STOPPED) {

          motorArray[i].change_state(STOPPED);
        }
        break;

      case DIRECTION_2:
        if (motorArray[i].read() == DIRECTION_1 ||
            motorArray[i].read() == STOPPED) {

          motorArray[i].change_state(STOPPED);
        }
        break;

      case STOPPED:
      default:
        if (motorArray[i].read() == DIRECTION_1) {
          motorArray[i].change_state(DIRECTION_1);
        }else if (motorArray[i].read() == DIRECTION_2) {
         motorArray[i].change_state(DIRECTION_2);
        }
        break;
      }
    }
  }
}

//=====[Implementations of private functions]==================================
