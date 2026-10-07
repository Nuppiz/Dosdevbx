#include "Common.h"
#include "Defines.h"

/* Source: "Tricks of the Game Programming Gurus" by Sams Publishing, authored by André LaMothe, John Ratcliff, Denise Tyler and Mark Seminatore, 1994 */

uint32_t program_time;

static void interrupt (far *old_Timer_ISR)(void);

void interrupt far Timer(void)
{
    static long last_clock_time = 0;

    program_time++;

    // keeps the PC clock ticking in the background
    if (last_clock_time + 182 < program_time)
    {
        last_clock_time = program_time;
        old_Timer_ISR();
    }
}

/* The original IBM PC ran the Intel 8253 PIT at 1.19318 MHz, 1/3 of the NTSC colour burst signal. This was necessary to make it easy to hook up an ordinary TV set to the PC CGA composite output.
   This was then further slowed down by dividing it with 65535, the largest 16-bit value, to turn it into the standard DOS timer interrupt value of 18.2 Hz. Reprogramming this exact divisor is what
   gives us a 1 kHz timer to tap into. */
void setTimer(uint16_t new_count)
{
    outportb(CONTROL_8253, CONTROL_WORD);
    outportb(COUNTER_0, LOW_BYTE(new_count));
    outportb(COUNTER_0, HIGH_BYTE(new_count));
}

void initTimer()
{
    old_Timer_ISR = _dos_getvect(TIME_KEEPER_INT);
    _dos_setvect(TIME_KEEPER_INT, Timer);
    setTimer(TIMER_1000HZ);
}

void deInitTimer()
{
    setTimer(TIMER_18HZ);
    _dos_setvect(TIME_KEEPER_INT, old_Timer_ISR);
}