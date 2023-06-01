//=====[#include guards - begin]===============================================

#ifndef _FEEDER_H_
#define _FEEDER_H_

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

typedef enum {
    FEEDER_CLOSED,
    FEEDER_OPEN,
    FEEDER_OPENING,
    FEEDER_CLOSING,
} feederStatus_t;

//=====[Declarations (prototypes) of public functions]=========================

void feederInit();
void feederUpdate()


feederStatus_t feederStatusRead();

//=====[#include guards - end]=================================================

#endif // _FEEDER_H_
