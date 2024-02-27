//=====[Libraries]=============================================================

#include "mbed.h"

#include "ble_com.h"
#include "pc_serial_com.h"
#include "arm_book_lib.h"
#include "date_and_time.h"

#include "mbed.h"




#ifdef _PROBANDO_BLE
DigitalOut z1(LED1);
DigitalOut z2(LED2);
DigitalOut z3(LED3);
#endif  

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================




//=====[Declaration and initialization of public global objects]===============

UnbufferedSerial uartBle(PD_5, PD_6, 9600);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================



//=====[Implementations of public functions]===================================

void bleComUpdate()
{
}

void bleComStringWrite( const char* str )
{
    uartBle.write( str, strlen(str) );
}

//=====[Implementations of private functions]==================================

char bleComCharRead()
{
    char receivedChar = '\0';
    if( uartBle.readable() ) {
        uartBle.read(&receivedChar,1);
    }
    return receivedChar;
}




