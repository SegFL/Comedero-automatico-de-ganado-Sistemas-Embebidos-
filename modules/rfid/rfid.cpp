#include "mbed.h"
#include "SPI.h"
#include "MFRC522.h"
#include "arm_book_lib.h"
#include "non_blocking_delay.h"
#include "rfid.h"

#define RFID_MF_RESET PE_3
#define RFID_SPI_MOSI PE_6
#define RFID_SPI_MISO PE_5
#define RFID_SPI_SCK PE_2
#define RFID_SPI_CS PB_4        //chip select


DigitalOut LedGreen(LED1);
//Serial pc(USBTX,USBRX);



//private objects
static nonBlockingDelay_t rfid_delay;
static rfidStatus_t rfidStatus=RFID_IDLE;


MFRC522 RfChip(RFID_SPI_MOSI, RFID_SPI_MISO, RFID_SPI_SCK, RFID_SPI_CS, RFID_MF_RESET);




void rfidInit(){

    tickInit();
    RfChip.PCD_Init();

}


void rfidUpdate(){

    if ( ! RfChip.PICC_IsNewCardPresent()){

        LedGreen.write(OFF);
/*
        if(rfidStatus==RFID_IDLE){
            nonBlockingDelayInit(&rfid_delay,500);
            rfidStatus=RFID_READING_NEW_CARD;
            return;
        }
        
        if(rfidStatus==RFID_READING_NEW_CARD){

            if(nonBlockingDelayRead(&rfid_delay)){

            }
        }
            return;
            */
    }
/*
    

    if ( ! RfChip.PICC_ReadCardSerial()){

    }

    for (uint8_t i = 0; i < RfChip.uid.size; i++){

        printf(" %X02", RfChip.uid.uidByte[i]);
        
    */

}