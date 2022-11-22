#ifndef CB_TX1_H
#define	CB_TX1_H

void SendMessage(unsigned char* message, int length);
void CB_TX1_Add(unsigned char value);
void SendOne();
unsigned char CB_TX1_IsTranmitting(void);
int CB_TX1_RemainingSize(void);

#endif	/* CB_TX1_H */

