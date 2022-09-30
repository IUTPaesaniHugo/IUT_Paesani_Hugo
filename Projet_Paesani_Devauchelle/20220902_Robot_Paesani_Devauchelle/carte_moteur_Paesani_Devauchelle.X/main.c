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
unsigned char stateRobot;

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
    InitTimer4();

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
        if (ADCIsConversionFinished()) {
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
            volts = ((float) result [4])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExGauche = 34 / volts - 5;

            if (robotState.distanceTelemetreDroit < 30) {
                LED_ORANGE = 1;
            } else {
                LED_ORANGE = 0;
            }

            if (robotState.distanceTelemetreCentre < 30) {
                LED_BLEUE = 1;
            } else {
                LED_BLEUE = 0;
            }

            if (robotState.distanceTelemetreGauche < 30) {
                LED_BLANCHE = 1;
            } else {
                LED_BLANCHE = 0;
            }
        }
    }
}// fin main

void OperatingSystemLoop(void) {
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;

        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;

        case STATE_AVANCE:
            PWMSetSpeedConsigne(25, MOTEUR_DROIT);
            PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(16, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(16, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;

    //D�termination de la position des obstacles en fonction des t�l�m�tres
    if (((stateRobot != (STATE_AVANCE)) || (stateRobot != (STATE_AVANCE_EN_COURS))) &&
            ((robotState.distanceTelemetreDroit < 45 &&
            robotState.distanceTelemetreCentre > 35 &&
            robotState.distanceTelemetreGauche > 35) ||
            (robotState.distanceTelemetreExDroit < 20 &&
            robotState.distanceTelemetreGauche > 35) ||
            (robotState.distanceTelemetreExDroit < 10)))//Obstacle � droite pas en vitesse de pointe
        positionObstacle = OBSTACLE_A_DROITE;
    if (((stateRobot == (STATE_AVANCE)) || (stateRobot == (STATE_AVANCE_EN_COURS))) &&
            ((robotState.distanceTelemetreDroit < 55 &&
            robotState.distanceTelemetreCentre > 45 &&
            robotState.distanceTelemetreGauche > 45) ||
            (robotState.distanceTelemetreExDroit < 30 &&
            robotState.distanceTelemetreGauche > 45) ||
            (robotState.distanceTelemetreExDroit < 20)))//Obstacle � droite pas en vitesse de pointe
        positionObstacle = OBSTACLE_A_DROITEVP;
    else if (((stateRobot == (STATE_AVANCE)) || (stateRobot == (STATE_AVANCE_EN_COURS))) &&
            ((robotState.distanceTelemetreDroit > 45 &&
            robotState.distanceTelemetreCentre > 45 &&
            robotState.distanceTelemetreGauche < 55) ||
            (robotState.distanceTelemetreDroit > 55 &&
            robotState.distanceTelemetreExGauche < 30) ||
            (robotState.distanceTelemetreExGauche < 20))) //Obstacle � gauche
        positionObstacle = OBSTACLE_A_GAUCHEVP;
    else if (((stateRobot != (STATE_AVANCE)) || (stateRobot != (STATE_AVANCE_EN_COURS))) &&
            ((robotState.distanceTelemetreDroit > 35 &&
            robotState.distanceTelemetreCentre > 35 &&
            robotState.distanceTelemetreGauche < 45) ||
            (robotState.distanceTelemetreDroit > 35 &&
            robotState.distanceTelemetreExGauche < 20) ||
            (robotState.distanceTelemetreExGauche < 10))) //Obstacle � gauche pas en vitesse de pointe
        positionObstacle = OBSTACLE_A_GAUCHE;
    else if (robotState.distanceTelemetreCentre < 45) //Obstacle en face
        positionObstacle = OBSTACLE_EN_FACE;
    else if (robotState.distanceTelemetreDroit > 55 &&
            robotState.distanceTelemetreCentre > 45 &&
            robotState.distanceTelemetreGauche > 35) //pas d?obstacle
        positionObstacle = PAS_D_OBSTACLE;

    //D�termination de l?�tat � venir du robot
    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_EN_FACE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;

    //Si l'on n?est pas dans la transition de l?�tape en cours
    if (nextStateRobot != stateRobot - 1) {
        stateRobot = nextStateRobot;
    }
}