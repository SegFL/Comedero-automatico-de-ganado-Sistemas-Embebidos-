#include "mbed.h"
#include "SPI.h"
#include "MFRC522.h"
#include "arm_book_lib.h"
#include "non_blocking_delay.h"
#include "rfid.h"
#include "string.h"
#include "aux_functions.h"
#include "feeder.h"

typedef enum{
    RFID_IDLE,
    RFID_READING_NEW_CARD,
    RFID_VALID_CARD
} rfidStatus_t;

//private objects
static nonBlockingDelay_t rfid_delay;
static rfidStatus_t rfidStatus=RFID_IDLE;

static char buffer[20]={'\0'};
MFRC522 RfChip(RFID_SPI_MOSI, RFID_SPI_MISO, RFID_SPI_SCLK, RFID_SPI_CS, RFID_MF_RESET);


#ifdef _PROBANDO_RFID
#define _PROBANDO_RFID
DigitalOut LedGreen(LED1);
DigitalOut LedBlue(LED2);
DigitalOut LedRed(LED3);

#endif

void rfidInit(){

    tickInit();
    RfChip.PCD_Init();

}


void rfidUpdate(){

    switch(rfidStatus){
        case RFID_IDLE:{
            if ( ! RfChip.PICC_IsNewCardPresent()) 
		        return;
            rfidStatus=RFID_READING_NEW_CARD;
	        break;
        }
        case RFID_READING_NEW_CARD:{
            // Select one of the cards
            if ( RfChip.PICC_ReadCardSerial()==false) {
                return;
	        }     
            for (uint8_t i = 0; i < RfChip.uid.size; i++)
            {
                 sprintf(buffer+i*2,"%02X", RfChip.uid.uidByte[i]);
            } 
            rfidStatus=RFID_VALID_CARD;
            break;
        }
        case RFID_VALID_CARD:{

           // puts("tengo una uid valida");
            break;
        }

        }

}
//Se supone que borran la memoria
char* rfidGetUid(){
    if(rfidStatus!=RFID_VALID_CARD)
        return NULL;

    char* aux=strndup(buffer,10);
    buffer[0]={'\0'};
    rfidStatus=RFID_IDLE;//Ya procese el uid ingresado a si que vuelvo a modo de esepra
    return aux;

}