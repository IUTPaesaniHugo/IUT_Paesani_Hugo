#include <xc.h> // library xc.h inclut tous les uC
#include "IO.h"
#include "asservissement.h"
#include "main.h"
#include "Utilities.h"
#include "UART_Protocol.h"
#include "Robot.h"

double proportionelleMax;
double integralMax;
double deriveeMax;

void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd){
PidCorr->Kp = Kp;
PidCorr->erreurProportionelleMax = proportionelleMax; //On limite la correction due au Kp
PidCorr->Ki = Ki;
PidCorr->erreurIntegraleMax = integralMax; //On limite la correction due au Ki
PidCorr->Kd = Kd;
PidCorr->erreurDeriveeMax = deriveeMax;
}

void SendPidAsservissement(volatile PidCorrector* PidCorr){
    double Kp, Ki, Kd, proportionelleMax, integralMax, deriveeMax;
    unsigned char message[29];
    Kp=PidCorr->Kp;
    Ki=PidCorr->Ki;
    Kd=PidCorr->Kd;
    proportionelleMax=PidCorr->erreurProportionelleMax;
    integralMax=PidCorr->erreurIntegraleMax;
    deriveeMax=PidCorr->erreurDeriveeMax;
    if(PidCorr==&robotState.PidX){
        message[0]=(unsigned char)0;
    }
    else{
        message[0]=(unsigned char)1;
    }   
    getBytesFromFloat(message, 1, Kp);
    getBytesFromFloat(message, 5, Ki);
    getBytesFromFloat(message, 9, Kd);
    getBytesFromFloat(message, 13, proportionelleMax);
    getBytesFromFloat(message, 17, integralMax);
    getBytesFromFloat(message, 21, deriveeMax);
    
    UartEncodeAndSendMessage(0x0063, 29, message);  
}