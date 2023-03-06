#include <xc.h> // library xc.h inclut tous les uC
#include "IO.h"
#include "asservissement.h"
#include "main.h"
#include "Utilities.h"
#include "UART_Protocol.h"
#include "Robot.h"
#include "QEI.h"
#include "ToolBox.h"

double proportionelleMax;
double integralMax;
double deriveeMax;

void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double consigne) {
    PidCorr->Kp = Kp;
    PidCorr->erreurProportionelleMax = proportionelleMax; //On limite la correction due au Kp
    PidCorr->Ki = Ki;
    PidCorr->erreurIntegraleMax = integralMax; //On limite la correction due au Ki
    PidCorr->Kd = Kd;
    PidCorr->erreurDeriveeMax = deriveeMax;
    PidCorr->consigne = consigne;
    PidCorr->commande = 12.90;
    PidCorr->erreur = PidCorr->commande * 3;
    PidCorr->corrP = 20.75;
    PidCorr->corrI = 2.19;
    PidCorr->corrD = 9.11;


}

void SendPidAsservissement(volatile PidCorrector* PidCorr) {
    double Kp, Ki, Kd, proportionelleMax, integralMax, deriveeMax, consigne, corrP, corrI, corrD;
    double erreur, commande;
    unsigned char message[49];
    Kp = PidCorr->Kp;
    Ki = PidCorr->Ki;
    Kd = PidCorr->Kd;
    proportionelleMax = PidCorr->erreurProportionelleMax;
    integralMax = PidCorr->erreurIntegraleMax;
    deriveeMax = PidCorr->erreurDeriveeMax;
    consigne = PidCorr->consigne;
    erreur = PidCorr->erreur;
    commande = PidCorr->commande;
    corrP = PidCorr->corrP;
    corrI = PidCorr->corrI;
    corrD = PidCorr->corrD;


    if (PidCorr == &robotState.PidX) {
        message[0] = (unsigned char) 0;
    } else {
        message[0] = (unsigned char) 1;
    }
    getBytesFromFloat(message, 1, Kp);
    getBytesFromFloat(message, 5, Ki);
    getBytesFromFloat(message, 9, Kd);
    getBytesFromFloat(message, 13, proportionelleMax);
    getBytesFromFloat(message, 17, integralMax);
    getBytesFromFloat(message, 21, deriveeMax);
    getBytesFromFloat(message, 25, consigne);
    getBytesFromFloat(message, 29, erreur);
    getBytesFromFloat(message, 33, commande);
    getBytesFromFloat(message, 37, corrP);
    getBytesFromFloat(message, 41, corrI);
    getBytesFromFloat(message, 45, corrD);


    UartEncodeAndSendMessage(0x0063, 49, message);
}

void SendCorrAsservissement(volatile PidCorrector* PidCorr) {
    
    
}



double Correcteur(volatile PidCorrector* PidCorr, double erreur) {
    PidCorr->erreur = erreur;
    double erreurProportionnelle = LimitToInterval(PidCorr->erreur, -PidCorr->erreurProportionelleMax / PidCorr->Kp, PidCorr->erreurProportionelleMax / PidCorr->Kp);
    PidCorr->corrP = PidCorr->Kp*erreurProportionnelle;

    PidCorr->erreurIntegrale += PidCorr->erreur / FREQ_ECH_QEI;
    PidCorr->erreurIntegrale = LimitToInterval(PidCorr->erreurIntegrale, -PidCorr->erreurIntegraleMax / PidCorr->Ki, PidCorr->erreurIntegraleMax / PidCorr->Ki);
    PidCorr->corrI = PidCorr->Ki * PidCorr->erreurIntegrale;
    
    double erreurDerivee = (erreur - PidCorr->epsilon_1) * FREQ_ECH_QEI;
    double deriveeBornee = LimitToInterval(erreurDerivee, -PidCorr->erreurDeriveeMax / PidCorr->Kd, PidCorr->erreurDeriveeMax / PidCorr->Kd);
    PidCorr->epsilon_1 = erreur;
    PidCorr->corrD = deriveeBornee * PidCorr->Kd;
    return PidCorr->corrP + PidCorr->corrI + PidCorr->corrD;
}

void UpdateAsservissement() {
    double erreurLin = robotState.PidX.consigne - robotState.vitesseLineaireFromOdometry;
    robotState.xCorrectionVitessePourcent = Correcteur(&robotState.PidX, erreurLin);
    
    double erreurAng = robotState.PidTheta.consigne - robotState.vitesseAngulaireFromOdometry;
    robotState.thetaCorrectionVitessePourcent = Correcteur(&robotState.PidTheta, erreurAng);
    
    
    PWMSetSpeedConsignePolaire(robotState.xCorrectionVitessePourcent,
            robotState.thetaCorrectionVitessePourcent);
}