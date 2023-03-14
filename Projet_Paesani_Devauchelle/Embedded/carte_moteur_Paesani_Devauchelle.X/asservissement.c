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
}

void SendPidAsservissementCst(volatile PidCorrector* PidCorr) {
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

double Correcteur(volatile PidCorrector* PidCorr, double erreur) {
    PidCorr->erreur = erreur;
    double erreurProportionnelle = LimitToInterval(PidCorr->erreur, -PidCorr->erreurProportionelleMax / PidCorr->Kp, PidCorr->erreurProportionelleMax / PidCorr->Kp);
    PidCorr->corrP = PidCorr->Kp*erreurProportionnelle;

    PidCorr->erreurIntegrale = PidCorr->erreurIntegrale + (PidCorr->erreur / FREQ_ECH_QEI);
    //PidCorr->erreurIntegrale = LimitToInterval(PidCorr->erreurIntegrale, -PidCorr->erreurIntegraleMax / PidCorr->Ki, PidCorr->erreurIntegraleMax / PidCorr->Ki);
    PidCorr->corrI = PidCorr->Ki * PidCorr->erreurIntegrale;

    double erreurDerivee = (erreur - PidCorr->epsilon_1) * FREQ_ECH_QEI;
    double deriveeBornee = LimitToInterval(erreurDerivee, -PidCorr->erreurDeriveeMax / PidCorr->Kd, PidCorr->erreurDeriveeMax / PidCorr->Kd);

    PidCorr->corrD = deriveeBornee * PidCorr->Kd; // erreurDerivee * PidCorr->Kd;
    double seuil = 2;
    if (PidCorr->corrD > seuil)
        PidCorr->corrD -= seuil;
    else if (PidCorr->corrD<-seuil)
        PidCorr->corrD += seuil;
    else
        PidCorr->corrD = 0;

    PidCorr->epsilon_1 = erreur;
    return PidCorr->corrP + PidCorr->corrI + PidCorr->corrD;
}

void UpdateAsservissement() {
    robotState.PidX.erreur = robotState.PidX.consigne - robotState.vitesseLineaireFromOdometry;
    robotState.vitesseLineaireCommand = Correcteur(&robotState.PidX, robotState.PidX.erreur);

    robotState.PidTheta.erreur = robotState.PidTheta.consigne - robotState.vitesseAngulaireFromOdometry;
    robotState.vitesseAngulaireCommand = Correcteur(&robotState.PidTheta, robotState.PidTheta.erreur);

    double CoeffVitesseToPercent = 50;
    robotState.vitesseGaucheConsigne = CoeffVitesseToPercent * (robotState.vitesseLineaireCommand - DISTROUES / 2 * robotState.vitesseAngulaireCommand);
    robotState.vitesseDroiteConsigne = CoeffVitesseToPercent * (robotState.vitesseLineaireCommand + DISTROUES / 2 * robotState.vitesseAngulaireCommand);
}

void SendVariableAsserv() {
    unsigned char message[40];
    getBytesFromFloat(message, 0, robotState.PidX.erreur);
    getBytesFromFloat(message, 4, robotState.PidTheta.erreur);
    getBytesFromFloat(message, 8, robotState.PidX.corrP + robotState.PidX.corrI + robotState.PidX.corrD);
    getBytesFromFloat(message, 12, robotState.PidTheta.corrP + robotState.PidTheta.corrI + robotState.PidTheta.corrD);
    getBytesFromFloat(message, 16, robotState.PidX.corrP);
    getBytesFromFloat(message, 20, robotState.PidTheta.corrP);
    getBytesFromFloat(message, 24, robotState.PidX.corrI);
    getBytesFromFloat(message, 28, robotState.PidTheta.corrI);
    getBytesFromFloat(message, 32, robotState.PidX.corrD);
    getBytesFromFloat(message, 36, robotState.PidTheta.corrD);

    UartEncodeAndSendMessage(0x0064, 40, message);
}