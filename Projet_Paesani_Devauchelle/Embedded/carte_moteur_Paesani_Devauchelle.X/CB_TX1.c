#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "CB_TX1.h"
#define CBTX1_BUFFER_SIZE 128

int cbTx1Head=0;
int cbTx1Tail=0;
unsigned char cbTx1Buffer[CBTX1_BUFFER_SIZE];
unsigned char isTransmitting = 0;

void SendMessage(unsigned char* message, int length)
{
unsigned char i=0;

if(CB_TX1_RemainingSize()>length)
{
//On peut écrire le message
for(i=0;i<length;i++)
CB_TX1_Add(message[i]);
if(!CB_TX1_IsTranmitting())
SendOne();
}
}

void CB_TX1_Add(unsigned char value)
{
    cbTx1Buffer[cbTx1Tail] = value;
    cbTx1Tail++;
    if(cbTx1Tail==CBTX1_BUFFER_SIZE){
        cbTx1Tail=0;
    }
}

unsigned char CB_TX1_Get(void)
{
    unsigned char c=cbTx1Buffer[cbTx1Head];
    cbTx1Buffer[cbTx1Head]=0;
    cbTx1Head++;
    if(cbTx1Head==CBTX1_BUFFER_SIZE){
        cbTx1Head=0;
    }
    return c;
} 

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
IFS0bits.U1TXIF = 0; // clear TX interrupt flag
if (cbTx1Tail!=cbTx1Head)
{
SendOne();
}
else
isTransmitting = 0;
}

void SendOne()
{
isTransmitting = 1;
unsigned char value=CB_TX1_Get();
U1TXREG = value; // Transmit one character
}

unsigned char CB_TX1_IsTranmitting(void)
{
    return isTransmitting;
}

int CB_TX1_RemainingSize(void)
{
int rSize;
unsigned char j=0;
for(j=0; j<CBTX1_BUFFER_SIZE; j++){
    if(cbTx1Buffer[j]==0){
        rSize++;
    }
}
return rSize;
}