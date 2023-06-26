//=====[#include guards - begin]===============================================

#ifndef _BLE_COM_H_
#define _BLE_COM_H_


#define SERVER_UUID 454fe39b-0cda-4836-b15d-1b9953faeefc
#define CARACTERISTIC_UUID 51c99e2a-7369-4f98-b4c4-e9b376e4e889
//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void bleComUpdate();
void bleComStringWrite( const char* str );

//=====[#include guards - end]=================================================

#endif // _BLE_COM_H_