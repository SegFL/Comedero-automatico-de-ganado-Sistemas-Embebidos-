#include "mbed.h"
#include "arm_book_lib.h"

#include "main_system.h"





int main()
{
    mainSystemInit();
    while (true) {
        mainSystemUpdate();
    }
}