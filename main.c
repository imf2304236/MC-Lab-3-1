#include <stdint.h>
#include "tm4c1294ncpdt.h"

void configSys(void);

void displayValue(unsigned char);

void configTimer(void);

void wait30us(void);

void main(void) {
    int i;
    static unsigned char convInput;

    configSys();

    while(1) {
        convInput = 0;    // Reset D/A Converter input byteword

        // Iterate from MSB to LSB of D/A Converter input
        for (i=7; i!=-1; --i) {
            while (GPIO_PORTD_AHB_DATA_R & 2ul);    // Wait while the Stop button is pressed
            GPIO_PORTK_DATA_R = (convInput | (uint32_t)1<<i); // Assert current bit
            wait30us();
            if (!(GPIO_PORTD_AHB_DATA_R & 1ul))     // If PORTD(0) reads LOW
                convInput &= ~(uint32_t)1<<i;       // Clear current bit of D/A Converter input
        }

        displayValue(convInput);
    }
}

void configSys(void)
{
    SYSCTL_RCGCGPIO_R |= 0x00000E08;            // Enable Ports M, L, K, D
    while (!(SYSCTL_PRGPIO_R & 0x00000E08ul));  // Wait for Ports Ready flag

    GPIO_PORTL_DIR_R |= 0x00000007;         // Configure PORTL(2:0) as outputs
    GPIO_PORTM_DIR_R |= 0x000000FF;         // Configure PORTM(7:0) as outputs
    GPIO_PORTD_AHB_DIR_R &= ~0x00000003ul;  // Configure PORTD(1:0) as inputs
    GPIO_PORTK_DIR_R |= 0x000000FF;         // Configure PORTK(7:0) as outputs

    GPIO_PORTL_DEN_R |= 0x00000007;         // Enable PORTL(2:0)
    GPIO_PORTM_DEN_R |= 0x000000FF;         // Enable PORTM(7:0)
    GPIO_PORTD_AHB_DEN_R |= 0x00000003;     // Enable PORTD(1:0)
    GPIO_PORTK_DEN_R |= 0x000000FF;         // Enable PORTK(7:0)

    configTimer();
}

void displayValue(unsigned char input)
{

}

void configTimer(void)
{
    SYSCTL_RCGCTIMER_R |= 1ul;  // Enable Timer 0 Module
    TIMER0_CTL_R &= ~1ul;       // Disable TIMER0A
    TIMER0_CFG_R |= 4ul;        // Set 16 bit mode
    TIMER0_TAPR_R = 0x00;       // Set Prescaler = 0
    TIMER0_TAMR_R |= 1ul;       // Set One shot mode
    TIMER0_TAMR_R &= ~0x10ul;   // Set Count down, Compare Mode
    TIMER0_TAILR_R = 480-1;     // Set Interval Load value = 480
}

void wait30us(void)
{
    TIMER0_CTL_R |= 1ul;            // Enable TIMER0A
    while (!(TIMER0_RIS_R & 1ul));  // Poll TIMER0A Time-Out Interrupt
    TIMER0_RIS_R |= 1ul;            // Clear TIMER0A Time-Out Interrupt
    TIMER0_CTL_R &= ~1ul;           // Disable TIMER0A
}