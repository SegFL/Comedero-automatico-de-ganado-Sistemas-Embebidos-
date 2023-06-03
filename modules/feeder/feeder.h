//=====[#include guards - begin]===============================================

#ifndef _FEEDER_H_
#define _FEEDER_H_

//=====[Declaration of public defines]=========================================
#define MOTOR_UPDATE_TIME 10
//=====[Declaration of public data types]======================================

typedef enum {
    FEEDER_FREE_MODE,
     FEEDER_MANUAL_MODE,
      FEEDER_TIME_MODE
} feederStatus_t;

//=====[Declarations (prototypes) of public functions]=========================

void feederInit();
void feederUpdate();


feederStatus_t feederStatusRead();

//=====[#include guards - end]=================================================

#endif // _FEEDER_H_
