#include <xc.h> // library xc.h inclut tous les uC
#include "IO.h"
#include "asservissement.h"
#include "main.h"
#include "Utilities.h"
#include "UART_Protocol.h"

double proportionelleMax;
double integralMax;
double deriveeMax;

void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double pro){
PidCorr->Kp = Kp;
PidCorr->erreurProportionelleMax = proportionelleMax; //On limite la correction due au Kp
PidCorr->Ki = Ki;
PidCorr->erreurIntegraleMax = integralMax; //On limite la correction due au Ki
PidCorr->Kd = Kd;
PidCorr->erreurDeriveeMax = deriveeMax;
}

void SendPidAsservissement(volatile PidCorrector* PidCorr, unsigned char dou){
    double Kp, Ki, Kd, Pro, proportionelleMax, integralMax, deriveeMax;
    unsigned char message[29];
    Kp=PidCorr->Kp;
    Ki=PidCorr->Ki;
    Kd=PidCorr->Kd;
    Pro=PidCorr->corrP;
    proportionelleMax=PidCorr->erreurProportionelleMax;
    integralMax=PidCorr->erreurIntegraleMax;
    deriveeMax=PidCorr->erreurDeriveeMax;
    
    message[0]=dou;    
    getBytesFromFloat(message, 1, Kp);
    getBytesFromFloat(message, 5, Ki);
    getBytesFromFloat(message, 9, Kd);
    getBytesFromFloat(message, 13, Pro);
    getBytesFromFloat(message, 17, proportionelleMax);
    getBytesFromFloat(message, 21, integralMax);
    getBytesFromFloat(message, 25, deriveeMax);
    
    UartEncodeAndSendMessage(0x0063, 29, message);  
}