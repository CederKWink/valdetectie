/*
 * clock.c
 *
 * Created: 8-5-2018 12:56:02
 *  Author: Robin
 */ 

#include <avr/io.h>


void Config32MHzClock(void)
{
	OSC.CTRL |= OSC_RC32MEN_bm;
	while (!(OSC.STATUS & OSC_RC32MRDY_bm)) ;
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
}
void Config32MHzClock_Ext16M(void)
{
	OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
	OSC.CTRL |= OSC_XOSCEN_bm;
	while ( ! (OSC.STATUS & OSC_XOSCRDY_bm) );
	OSC.PLLCTRL = OSC_PLLSRC_XOSC_gc | (OSC_PLLFAC_gm & 2);
	OSC.CTRL |= OSC_PLLEN_bm;
	while ( ! (OSC.STATUS & OSC_PLLRDY_bm) );
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_PLL_gc;
	OSC.CTRL &= ~OSC_RC2MEN_bm;
	OSC.CTRL &= ~OSC_RC32MEN_bm;
}