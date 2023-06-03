//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "feeder.h"
#include "motor.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

motor motorArray[] = {motor(PF_2, PE_3), motor(PH_0, PH_1)};

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============


static feederStatus_t feederStatus;

//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of public functions]===================================

void feederInit(){

    feederStatus=FEEDER_MANUAL_MODE;

}
void feederUpdate() {
  // Ambos motores tienen el mismo counter lo que singifica que ambos se
  // actualizan en el mismo ciclo.


    for (int i = 0; i < 2; i++) {

      switch (motorArray[i].read()) {
      case DIRECTION_1:
        if (motorArray[i].read() == DIRECTION_2 ||
            motorArray[i].read() == STOPPED) {

          motorArray[i].write(STOPPED);
        }
        break;

      case DIRECTION_2:
        if (motorArray[i].read() == DIRECTION_1 ||
            motorArray[i].read() == STOPPED) {

          motorArray[i].write(STOPPED);
        }
        break;

      case STOPPED:
      default:
        if (motorArray[i].read() == DIRECTION_1) {
          motorArray[i].write(DIRECTION_1);
        }else if (motorArray[i].read() == DIRECTION_2) {
         motorArray[i].write(DIRECTION_2);
        }
        break;
      }
    }

}

feederStatus_t feederStatusRead(){

    return feederStatus;

}