#include <msp430g2553.h>
#include <legacymsp430.h>
#include "LCD/LCD.h"
#include "game.h"

char timerFlag = 0;
unsigned char player;

void initTimer();
void clearTimer();
void initButtons();
void printWinScreen();
void printGameOverScreen();
void printBoomScreen();

int main(void)
{
	WDTCTL = (WDTPW|WDTHOLD);

	initSPI();
	LCDinit();
	LCDclear();

	unsigned char mines[NUM_MINES]; 
	generateMines(mines);
	printMines(mines);

	player = initPlayer();
	printPlayer(player);

	initTimer();
	initButtons();

	__enable_interrupt();

	while(1)
	{
		if (didPlayerWin(player))
		{
			printWinScreen();
			break;
		}

		if (timerFlag >= 4)
		{
			printGameOverScreen();
			break;
		}

		if (didPlayerHitMine(player, mines))
		{
			printBoomScreen();
			__delay_cycles(1000000);
			LCDclear();
			printGameOverScreen();
			break;
		}
	}

	__disable_interrupt();

	while(1){}

	return 0;
}

void printWinScreen()
{
	cursorToLineOne();
	writeString("YOU");
	cursorToLineTwo();
	writeString("WON!");
}

void printGameOverScreen()
{
	cursorToLineOne();
	writeString("GAME");
	cursorToLineTwo();
	writeString("OVER!");
}

void printBoomScreen()
{
	cursorToLineOne();
	writeString("BOOM!");
}

interrupt(TIMER0_A1_VECTOR) TIMER_A_ISR()
{
    TACTL &= ~TAIFG;            // clear interrupt flag
	timerFlag++;
}

void movePlayerInResponseToButtonPush(char buttonPushed)
{
	clearPlayer(player);

	switch (buttonPushed) {
		case BIT1:
			player = movePlayer(player, RIGHT);
			break;
		case BIT2:
			player = movePlayer(player, LEFT);
			break;
		case BIT3:
			player = movePlayer(player, UP);
			break;
		case BIT4:
			player = movePlayer(player, DOWN);
			break;
	}

	printPlayer(player);
}

void debounce()
{
	__delay_cycles(1000);
}

void testAndRespondToButtonPush(char buttonToTest)
{
	if (buttonToTest & P1IFG)
	{
		if (buttonToTest & P1IES)
		{
			movePlayerInResponseToButtonPush(buttonToTest);
			clearTimer();
		} else
		{
			debounce();
		}	
		
		P1IES ^= buttonToTest;
		P1IFG &= ~buttonToTest;
	}
}

interrupt(PORT1_VECTOR) PORT1_ISR()
{
	testAndRespondToButtonPush(BIT1);
	testAndRespondToButtonPush(BIT2);
	testAndRespondToButtonPush(BIT3);
	testAndRespondToButtonPush(BIT4);
}

void initTimer()
{
	TACTL &= ~(MC1|MC0);

	TACTL |= TACLR;

	TACTL |= TASSEL1;

	TACTL |= ID1|ID0;

	TACTL &= ~TAIFG;

	TACTL |= MC1;

	TACTL |= TAIE;
}

void clearTimer()
{
	TACTL |= TACLR;
	timerFlag = 0;
}

void initButtons()
{
	P1REN |= BIT1|BIT2|BIT3|BIT4;
	P1OUT |= BIT1|BIT2|BIT3|BIT4;
	P1IES |= BIT1|BIT2|BIT3|BIT4;			// select falling edge for each button
	P1IFG &= ~(BIT1|BIT2|BIT3|BIT4);		// clear flags
	P1IE |= BIT1|BIT2|BIT3|BIT4;			// enable interrupts
}
