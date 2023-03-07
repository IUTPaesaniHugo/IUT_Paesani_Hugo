#ifndef ASSERVISSEMENT_H
#define	ASSERVISSEMENT_H

typedef struct _PidCorrector
{
double Kp;
double Ki;
double Kd;
double erreurProportionelleMax;
double erreurIntegraleMax;
double erreurDeriveeMax;
double erreurProportionel;
double erreurIntegrale;
double erreurDerivee;
double consigne;
double commande;
double epsilon_1;
double erreur;
//For Debug only
double corrP;
double corrI;
double corrD;
}PidCorrector;

extern double proportionelleMax;
extern double integralMax;
extern double deriveeMax;

void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double consigne);
void SendPidAsservissementCst(volatile PidCorrector* PidCorr);
double Correcteur(volatile PidCorrector* PidCorr, double erreur);
void SendVariableAsserv();
void UpdateAsservissement();
#endif	/* ASSERVISSEMENT_H */

