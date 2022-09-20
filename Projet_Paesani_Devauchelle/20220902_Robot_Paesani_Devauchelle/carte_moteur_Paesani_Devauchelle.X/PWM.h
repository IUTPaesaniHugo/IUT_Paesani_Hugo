
#ifndef PWM_H
#define	PWM_H

#define MOTEUR_GAUCHE 0
#define MOTEUR_DROIT 1

float vitesseEnPourcents;
uint16_t moteur;

void InitPWM(void);
void PWMSetSpeed(float vitesseEnPourcents, uint16_t moteur);
void PWMUpdateSpeed();
void PWMSetSpeedConsigne(float vitesseEnPourcents, uint16_t moteur);


#endif	/* PWM_H */

