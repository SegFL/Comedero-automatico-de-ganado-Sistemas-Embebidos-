//=====[Libraries]=============================================================

#include "feeder.h"
#include "pc_serial_com.h"
#include "rfid.h"
#include "log.h"
#include "ble_com.h"
#include "main_system.h"
#include "non_blocking_delay.h"
//=====[Declaration of private defines]========================================



//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


static nonBlockingDelay_t mainDelay;
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
  nonBlockingDelayInit( &mainDelay, SYSTEM_TIME_INCREMENT_MS );

}

void mainSystemUpdate()
{
   
    if(nonBlockingDelayRead(&mainDelay)==true){
        feederUpdate();
        pcSerialComUpdate();
        nonBlockingDelayInit(&mainDelay,SYSTEM_TIME_INCREMENT_MS);
    }
    rfidUpdate();
}

//=====[Implementations of private functions]==================================
