//=====[#include guards - begin]===============================================

#ifndef _PC_SERIAL_COM_H_
#define _PC_SERIAL_COM_H_

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void userInterfaceInit();
char userInterfaceCharRead();
void userInterfaceStringWrite( const char* str );
void userInterfaceUpdate();
bool userInterfaceStateNewUid();
int commandGetFeederDirection();

//=====[#include guards - end]=================================================

#endif // _PC_SERIAL_COM_H_
