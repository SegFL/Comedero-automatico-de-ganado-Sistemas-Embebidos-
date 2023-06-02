#include "mbed.h"
#include "arm_book_lib.h"

/* Include statechart header file. Be sure you run the statechart C++ code
 * generation tool! */
#include "Statechart.h"
#include "sc_timer_service.h"
#include "sc_types.h"

using namespace std;
using namespace sc::timer;

/*! As we make use of time triggers (after & every)
 * we make use of a generic timer implementation
 * and need a defined number of timers. */
#define MAX_TIMERS 20

//! We allocate the desired array of timer tasks.
TimerTask tasks[MAX_TIMERS];

//! The timer tasks are managed by a timer service. */
TimerService *timerService = new TimerService(tasks, MAX_TIMERS);

/*! Instantiates the state machine */
Statechart *sm = new Statechart();


// The ticker (smTicker), the interval (smTickRate 1 mili second),
// the flag (smTickFlag) and the function to be attached (smTick) of this ticket
Ticker smTicker;

// sc_integer smTickRate = 1;

bool smTickFlag = false;
void smTick(void);

void smTick(void)
{  
	/* The smTick only set a Flag */
    smTickFlag = true;
}

 
int main()
{
	/*! Sets the timer service */
	sm->setTimerService( timerService );

    // the address of the function to be attached (smTick) and the interval (smTickRate 1 mili second)
    smTicker.attach( &smTick, 1ms ); 
    
	/*! Enters the state machine; from this point on the state machine is ready to react on incoming event */
	sm->enter();

    while (true) {

		/* Checking the waited Flag */
        if (smTickFlag == true) {
			/* Then reset its Flag */
            smTickFlag = false;
            
			/* Then Update all Time Events (smTickRate 1 mili second),
             * and then Run an Cycle of state machine */
			timerService->proceed( 1 );
        }
    }
}