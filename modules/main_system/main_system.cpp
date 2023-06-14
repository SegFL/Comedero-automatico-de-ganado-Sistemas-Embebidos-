//=====[Libraries]=============================================================

#include "feeder.h"
#include "pc_serial_com.h"
#include "rfid.h"
#include "log.h"
//=====[Declaration of private defines]========================================



//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============



//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void mainSystemInit()
{
    feederInit();
    pcSerialComInit();
    rfidInit();
    logInit();
  //nonBlockingDelayInit( &mainSystemDelay, SYSTEM_TIME_INCREMENT_MS );

}

void mainSystemUpdate()
{
   

    feederUpdate();
    pcSerialComUpdate();
    rfidUpdate();


}

//=====[Implementations of private functions]==================================
