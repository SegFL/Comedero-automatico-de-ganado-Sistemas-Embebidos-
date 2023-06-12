#include "mbed.h"
#include "SPI.h"
#include "MFRC522.h"
#include "arm_book_lib.h"
#include "non_blocking_delay.h"
#include "rfid.h"
#include "string.h"
#include "feeder.h"





//SPI DebugUART(UART_TX, UART_RX,115200);


//private objects
static nonBlockingDelay_t rfid_delay;
static rfidStatus_t rfidStatus=RFID_IDLE;


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



	if ( ! RfChip.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! RfChip.PICC_ReadCardSerial()) {
		return;
	}

    char* buffer=(char*)calloc(21,sizeof(char));
    for (uint8_t i = 0; i < RfChip.uid.size; i++)
    {
       sprintf(buffer+i*2,"%02X", RfChip.uid.uidByte[i]);
    } 
   feederFreeModeInit(buffer);
    free(buffer);

}