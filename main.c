#include <msp430g2553.h>
// if you need interrupts
#include <legacymsp430.h>

int main(void)
{
	WDTCTL = (WDTPW|WDTHOLD);

	__enable_interrupt();
	__disable_interrupt();

	while(1){}

	return 0;
}

interrupt(TIMER0_A1_VECTOR) TIMER_A_ISR()
{
	// sample ISR
}
