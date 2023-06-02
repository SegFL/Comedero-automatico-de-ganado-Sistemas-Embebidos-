//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "feeder.h"
#include "motor.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

InterruptIn gateOpenLimitSwitch(PG_1);
InterruptIn gateCloseLimitSwitch(PF_7);


motor motorArray[] = {motor(PF_2, PE_3), motor(PH_0, PH_1)};

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool gateOpenLimitSwitchState;
static bool gateCloseLimitSwitchState;

static feederStatus_t gateStatus;

//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of public functions]===================================

void feederUpdate() {
  // Ambos motores tienen el mismo counter lo que singifica que ambos se
  // actualizan en el mismo ciclo.
  static int feederUpdateCounter = 0;

  feederUpdateCounter++;

  if (feederUpdateCounter > MOTOR_UPDATE_TIME) {

    feederUpdateCounter = 0;
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