//=====[#include guards - begin]===============================================

#ifndef _RFID_H_
#define _RFID_H_
//==================[Libraries]=================================================


#define RFID_MF_RESET PD_15
#define RFID_SPI_MOSI PA_7
#define RFID_SPI_MISO PA_6
#define RFID_SPI_SCLK PA_5
#define RFID_SPI_CS PD_14      //chip select
//=====[Declaration of public data types]======================================

typedef enum{
    RFID_IDLE,
    RFID_READING_NEW_CARD,
    RFID_VALID_CARD
} rfidStatus_t;




//=====[Declarations (prototypes) of public functions]=========================
void rfidUpdate();
void rfidInit();
char* rfidGetUid();
//=====[#include guards - end]=================================================
#endif // _RFID_H_