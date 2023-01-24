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
#include "UART.h"
#include "CB_TX1.h"
#include "CB_RX1.h"
#include <libpic30.h>
#include "UART_Protocol.h"

unsigned int ADCValue0;
unsigned int ADCValue1;
unsigned int ADCValue2;
unsigned int ADCValue3;
unsigned int ADCValue4;
unsigned char stateRobot;
unsigned char payload[] = {'B', 'o', 'n', 'j', 'o', 'u', 'r'};
unsigned char distIR[3];
unsigned char etape[5];
unsigned char led[2];
unsigned long temps;
unsigned int GlDist = 0;

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
    //Initialisation UART
    /****************************************************************************************************/
    InitUART();
    
    /****************************************************************************************************/
    //Initialisation QEI
    /****************************************************************************************************/
    InitQEI1();
    InitQEI2();
    
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
//        SendMessage((unsigned char*)"Bonjour loopback", 16);
        int i;
        for(i=0; i<CB_RX1_GetDataSize(); i++){
            unsigned char c = CB_RX1_Get();
            UartDecodeMessage(c);
            
        }
//        UartEncodeAndSendMessage(0x0080, 7, payload);
//        __delay32(40000000);
        
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

            if (robotState.distanceTelemetreCentre < 10) {
                robotState.distanceTelemetreCentre = 10;
            }
            if (robotState.distanceTelemetreGauche < 10) {
                robotState.distanceTelemetreGauche = 10;
            }
            if (robotState.distanceTelemetreDroit < 10) {
                robotState.distanceTelemetreDroit = 10;
            }
            if (robotState.distanceTelemetreExGauche < 10) {
                robotState.distanceTelemetreExGauche = 10;
            }
            if (robotState.distanceTelemetreExDroit < 10) {
                robotState.distanceTelemetreExDroit = 10;
            }
            
//            distIR[0] = robotState.distanceTelemetreGauche;
//            distIR[1] = robotState.distanceTelemetreCentre;
//            distIR[2] = robotState.distanceTelemetreDroit;          
//            UartEncodeAndSendMessage(0x0030, 3, distIR);
            
            if (robotState.distanceTelemetreDroit < 30) {
                LED_ORANGE = 1;
            } else {
                LED_ORANGE = 0;
            }

            //if (robotState.distanceTelemetreCentre < 30) {
            //    LED_BLEUE = 1;
            //} else {
            //    LED_BLEUE = 0;
            //}

            if (robotState.distanceTelemetreGauche < 30) {
                LED_BLANCHE = 1;
            } else {
                LED_BLANCHE = 0;
            }
        }
        if ((robotState.vitesseGaucheCommandeCourante == 25) && (robotState.vitesseDroiteCommandeCourante == 25)) {
            LED_BLEUE = 1;
        } else {
            LED_BLEUE = 0;
        }
//        led[0]=0;
//        led[1]=LED_ORANGE;
//        UartEncodeAndSendMessage(0x0020, 2, led);
//        led[0]=1;
//        led[1]=LED_BLEUE;
//        UartEncodeAndSendMessage(0x0020, 2, led);
//        led[0]=2;
//        led[1]=LED_BLANCHE;
//        UartEncodeAndSendMessage(0x0020, 2, led);               
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
            if(autoControlActivated==1){
            SetNextRobotStateInAutomaticMode();
            }
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(13, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            if(autoControlActivated==1){
            SetNextRobotStateInAutomaticMode();
            }
            break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(13, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            if(autoControlActivated==1){
            SetNextRobotStateInAutomaticMode();
            }
            break;
            
        case STATE_TOURNE_GAUCHE_VNR:
            PWMSetSpeedConsigne(14, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS_VNR;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS_VNR:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE_VNR:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(14, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS_VNR;
            break;
        case STATE_TOURNE_DROITE_EN_COURS_VNR:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            if(autoControlActivated==1){
            SetNextRobotStateInAutomaticMode();
            }
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            if(autoControlActivated==1){
            SetNextRobotStateInAutomaticMode();
            }
            break;
        
        case STATE_RECULE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_RECULE_EN_COURS;
            break;
            
        case STATE_RECULE_EN_COURS:
            if(autoControlActivated==1){
            SetNextRobotStateInAutomaticMode();
            }
            break;    
            
        case STATE_ARRET:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_RECULE_EN_COURS;
            break;
            
         case STATE_ARRET_EN_COURS:
            if(autoControlActivated==1){
            SetNextRobotStateInAutomaticMode();
            }
            break;
            
            
        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;

    //Détermination de la position des obstacles en fonction des télémètres
    if (robotState.distanceTelemetreCentre < 25)
        positionObstacle = OBSTACLE_EN_FACE;

    else if (robotState.distanceTelemetreGauche < 25)
        positionObstacle = OBSTACLE_A_GAUCHE_VNR;

    else if (robotState.distanceTelemetreDroit < 25)
        positionObstacle = OBSTACLE_A_DROITE_VNR;

    else if (robotState.distanceTelemetreExDroit < 20)
        positionObstacle = OBSTACLE_A_DROITE;

    else if (robotState.distanceTelemetreExGauche < 20)
        positionObstacle = OBSTACLE_A_GAUCHE;

    else {
        switch (GlDist) {
            case 00000:
            case 10001: positionObstacle = PAS_D_OBSTACLE;
                break;

            case 00100:
            case 10101: if (robotState.distanceTelemetreDroit > robotState.distanceTelemetreGauche) {
                    positionObstacle = OBSTACLE_A_GAUCHE_VNR;
                } else {
                    positionObstacle = OBSTACLE_A_DROITE_VNR;
                }
                break;

            case 00001:
            case 00010:
            case 00011:
            case 10010:
            case 10011: positionObstacle = OBSTACLE_A_GAUCHE;
                break;
                
            case 10000:
            case 01000:
            case 11000:
            case 01001:
            case 11001: positionObstacle = OBSTACLE_A_DROITE;
                break;
                
            case 00101:
            case 00110:
            case 00111:
            case 10110:
            case 10111: positionObstacle = OBSTACLE_A_GAUCHE_VNR;
                break;
                
            case 10100:
            case 01100:
            case 11100:
            case 01101:
            case 11101: positionObstacle = OBSTACLE_A_DROITE_VNR;
                break;
                
            case 01010:
            case 01011:
            case 01110:
            case 01111:
            case 11011:
            case 11110:
            case 11010:
            case 11111: positionObstacle = OBSTACLE_EN_FACE;
                break;
        }
    }
    //Détermination de l'état à venir du robot
    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_A_DROITE_VNR)
        nextStateRobot = STATE_TOURNE_GAUCHE_VNR;
    else if (positionObstacle == OBSTACLE_A_GAUCHE_VNR)
        nextStateRobot = STATE_TOURNE_DROITE_VNR;
    else if (positionObstacle == OBSTACLE_EN_FACE) {
        if (robotState.distanceTelemetreExDroit >= robotState.distanceTelemetreExGauche) {
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        } else {
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        }
    }
    //Si l'on n?est pas dans la transition de l?étape en cours
    if (nextStateRobot != stateRobot - 1) {      
        stateRobot = nextStateRobot;
        temps = timestamp;
        etape[0]=stateRobot;
        etape[1]=temps>>24;
        etape[2]=temps>>16;
        etape[3]=temps>>8;
        etape[4]=temps;
        UartEncodeAndSendMessage(0x0050, 5, etape);
    }
}