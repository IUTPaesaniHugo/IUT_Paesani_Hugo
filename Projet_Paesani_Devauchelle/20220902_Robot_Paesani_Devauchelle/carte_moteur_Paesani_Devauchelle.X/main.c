#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include  "ChipConfig.h"
#include  "IO.h"
#include "timer.h"
#include "robot.h"
#include "ToolBox.h"
#include "PWM.h"

int main(void) {
    /***************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();
    
    /****************************************************************************************************/
    // Configuration des entr�es sorties
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
     /****************************************************************************************************/
    
    InitPWM();
    PWMSetSpeed(0);
 
    
    
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
    } 
}// fin main
