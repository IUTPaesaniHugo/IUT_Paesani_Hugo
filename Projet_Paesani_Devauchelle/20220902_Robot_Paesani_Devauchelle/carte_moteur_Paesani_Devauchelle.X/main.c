#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include  "ChipConfig.h"
#include  "IO.h"
#include "timer.h"
#include "robot.h"
#include "ToolBox.h"
#include "PWM.h"
#include "adc.h"
#include "main.h"

unsigned int ADCValue0;
unsigned int ADCValue1;
unsigned int ADCValue2;
unsigned int ADCValue3;
unsigned int ADCValue4;

int main(void) {
    /***************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();

    /****************************************************************************************************/
    // Configuration des entrées sorties
    /****************************************************************************************************/
    InitIO();

    /****************************************************************************************************/
    // Initialisation temporisations
    /****************************************************************************************************/
    InitTimer23();
    InitTimer1();

    LED_BLANCHE = 1;
    LED_BLEUE = 1;
    LED_ORANGE = 1;


    /****************************************************************************************************/
    // Initialisation temporisations
    /****************************************************************************************************/
    InitPWM();
    
    /****************************************************************************************************/
    // Initialisation Convertisseur ADC
    /****************************************************************************************************/
    InitADC1();
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
        if(ADCIsConversionFinished())
        {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            ADCValue0 = result[0];
            ADCValue1 = result[1];
            ADCValue2 = result[2];
            ADCValue1 = result[3];
            ADCValue2 = result[4];
            
            
            float volts = ((float) result [0])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExDroit = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [4])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExGauche = 34 / volts - 5;
            
            if((robotState.distanceTelemetreDroit < 30) || (robotState.distanceTelemetreExDroit < 30)){
                LED_ORANGE = 1;
            }
            else{
                LED_ORANGE = 0;
            }
            
            if(robotState.distanceTelemetreCentre < 30){
                LED_BLEUE = 1;
            }
            else{
                LED_BLEUE = 0;
            }
            
            if((robotState.distanceTelemetreGauche < 30) || (robotState.distanceTelemetreExGauche < 30)){
                LED_BLANCHE = 1;
            }
            else{
                LED_BLANCHE = 0;
            }
        }
    }
}// fin main
