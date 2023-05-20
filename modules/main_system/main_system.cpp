//=====[Libraries]=============================================================

#include "motor.h"
#include "wifi_com.h"
#include "non_blocking_delay.h"

//=====[Declaration of private defines]========================================


#define SYSTEM_TIME_INCREMENT_MS 10

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

static nonBlockingDelay_t mainSystemDelay;

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void mainSystemInit()
{
   
   motorControlInit();

   nonBlockingDelayInit( &mainSystemDelay, SYSTEM_TIME_INCREMENT_MS );

}

void mainSystemUpdate()
{
    motorControlUpdate();

    if( nonBlockingDelayRead(&mainSystemDelay) ) {

    }
    wifiComUpdate(); 
}

//=====[Implementations of private functions]==================================
