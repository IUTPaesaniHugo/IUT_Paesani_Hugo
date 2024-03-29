#include <xc.h>
#include "timer.h"
#include "IO.h"
#include "PWM.h"
#include "adc.h"
#include "main.h"
#include "UART_Protocol.h"
#include "QEI.h"
#include "asservissement.h"

int compt=0;
unsigned long timestamp=0;
//Initialisation d?un timer 32 bits

void InitTimer23(void) {
    T3CONbits.TON = 0; // Stop any 16-bit Timer3 operation
    T2CONbits.TON = 0; // Stop any 16/32-bit Timer3 operation
    T2CONbits.T32 = 1; // Enable 32-bit Timer mode
    T2CONbits.TCS = 0; // Select internal instruction cycle clock
    T2CONbits.TCKPS = 0b00; // Select 1:8 Prescaler
    TMR3 = 0x00; // Clear 32-bit Timer (msw)
    TMR2 = 0x00; // Clear 32-bit Timer (lsw)
    PR3 = 0x0BEB; // Load 32-bit period value (msw)
    PR2 = 0xC200; // Load 32-bit period value (lsw)
    IPC2bits.T3IP = 0x01; // Set Timer3 Interrupt Priority Level
    IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
    IEC0bits.T3IE = 1; // Enable Timer3 interrupt
    T2CONbits.TON = 1; // Start 32-bit Timer
    /* Example code for Timer3 ISR */
}

//Interruption du timer 32 bits sur 2-3

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) { //0,2Hz
    IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
    //LED_ORANGE = !LED_ORANGE;
    //if (tour == 0) {
    //  PWMSetSpeed(30, MOTEUR_DROIT);
    //  PWMSetSpeed(30, MOTEUR_GAUCHE);
    //  tour = 1;
    //} else {
    //    PWMSetSpeed(-30, MOTEUR_DROIT);
    //    PWMSetSpeed(-30, MOTEUR_GAUCHE);
    //    tour = 0;
    //}
    //  if(tour == 0){
    //      PWMSetSpeedConsigne(37, MOTEUR_GAUCHE);
    //      PWMSetSpeedConsigne(37, MOTEUR_DROIT);
    //      tour = 1;
    //  }
    //  else{
    //      PWMSetSpeedConsigne(-37, MOTEUR_GAUCHE);
    //      PWMSetSpeedConsigne(-37, MOTEUR_DROIT);
    //      tour=0;
    //  }
}

//Initialisation d?un timer 16 bits

void InitTimer1(void) { //Fr�quence de 250Hz

    SetFreqTimer1(250);
    //Timer1 pour horodater les mesures (1ms)
    T1CONbits.TON = 0; // Disable Timer
    //11 = 1:256 prescale value
    //10 = 1:64 prescale value
    //01 = 1:8 prescale value
    //00 = 1:1 prescale value
    T1CONbits.TCS = 0; //clock source = internal clock

    IFS0bits.T1IF = 0; // Clear Timer Interrupt Flag
    IEC0bits.T1IE = 1; // Enable Timer interrupt
    T1CONbits.TON = 1; // Enable Timer
}

//Interruption du timer 1

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0;
    //LED_BLANCHE = !LED_BLANCHE;
    PWMUpdateSpeed();
    ADC1StartConversionSequence();
    QEIUpdateData();
    UpdateAsservissement();
    compt++;
    if(compt%25==0){
        SendPositionData();
        SendVariableAsserv();
    }
}

void InitTimer4(void) { //Fr�quence de 150Hz

    SetFreqTimer4(1000);
    //Timer1 pour horodater les mesures (1ms)
    T4CONbits.TON = 0; // Disable Timer
    //11 = 1:256 prescale value
    //10 = 1:64 prescale value
    //01 = 1:8 prescale value
    //00 = 1:1 prescale value
    T4CONbits.TCS = 0; //clock source = internal clock

    IFS1bits.T4IF = 0; // Clear Timer Interrupt Flag
    IEC1bits.T4IE = 1; // Enable Timer interrupt
    T4CONbits.TON = 1; // Enable Timer
}

//Interruption du timer 1

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void) {
    IFS1bits.T4IF = 0;
    timestamp = timestamp + 1;
    OperatingSystemLoop();
}


void SetFreqTimer4(float freq) {
    T4CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
    if (FCY / freq > 65535) {
        T4CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
        if (FCY / freq / 8 > 65535) {
            T4CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
            if (FCY / freq / 64 > 65535) {
                T4CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                PR4 = (int) (FCY / freq / 256);
            } else
                PR4 = (int) (FCY / freq / 64);
        } else
            PR4 = (int) (FCY / freq / 8);
    } else
        PR4 = (int) (FCY / freq);
}

void SetFreqTimer1(float freq) {
    T1CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
    if (FCY / freq > 65535) {
        T1CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
        if (FCY / freq / 8 > 65535) {
            T1CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
            if (FCY / freq / 64 > 65535) {
                T1CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                PR1 = (int) (FCY / freq / 256);
            } else
                PR1 = (int) (FCY / freq / 64);
        } else
            PR1 = (int) (FCY / freq / 8);
    } else
        PR1 = (int) (FCY / freq);
}