#include "mbed.h"

#include "main_system.h"

// main() runs in its own thread in the OS
int main()
{

    mainSystemInit();
    while (true) {

        mainSystemUpdate();
    }
}

