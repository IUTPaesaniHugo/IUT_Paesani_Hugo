
#ifndef PWM_H
#define	PWM_H

#define MOTEUR_GAUCHE 0
#define MOTEUR_DROIT 1


void InitPWM(void);
void PWMSetSpeed(float vitesseEnPourcents, char moteur);
void PWMUpdateSpeed();
void PWMSetSpeedConsigne(float vitesseEnPourcents, char moteur);


#endif	/* PWM_H */

