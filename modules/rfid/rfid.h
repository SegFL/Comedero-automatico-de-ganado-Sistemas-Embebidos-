//=====[#include guards - begin]===============================================

#ifndef _RFID_H_
#define _RFID_H_
//==================[Libraries]=================================================


//=====[Declaration of public data types]======================================

typedef enum{
    RFID_IDLE,
    RFID_READING_NEW_CARD
} rfidStatus_t;




//=====[Declarations (prototypes) of public functions]=========================
void rfidUpdate();
void rfidInit();
//=====[#include guards - end]=================================================
#endif // _RFID_H_