#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"

unsigned char UartCalculateChecksum(int msgFunction,
        int msgPayloadLength, unsigned char* msgPayload)//Fonction prenant entree la trame et sa longueur pour calculer le checksum
{
    int i;
    unsigned char checksum = 0;
    checksum ^= 0xFE;
    checksum ^= (unsigned char) (msgFunction >> 8);
    checksum ^= (unsigned char) (msgFunction >> 0);
    checksum ^= (unsigned char) (msgPayloadLength >> 8);
    checksum ^= (unsigned char) (msgPayloadLength >> 0);

    for (i = 0; i < msgPayloadLength; i++) {
        checksum ^= msgPayload[i];
    }
    return checksum;
}

void UartEncodeAndSendMessage(int msgFunction,
        int msgPayloadLength, unsigned char* msgPayload) { //Fonction d?encodage et d?envoi d?un message
    unsigned char message [msgPayloadLength + 6];
    int pos = 0;
    message[pos++] = 0xFE;
    message[pos++] = (unsigned char) (msgFunction >> 8);
    message[pos++] = (unsigned char) (msgFunction >> 0);
    message[pos++] = (unsigned char) (msgPayloadLength >> 8);
    message[pos++] = (unsigned char) (msgPayloadLength >> 0);
    int i;
    for (i = 0; i < msgPayloadLength; i++) {
        message[pos++] = msgPayload[i];
    }

    message[pos] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
    SendMessage(message, msgPayloadLength + 6);
}

int rcvState = WAITING;
int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;
unsigned char receivedChecksum;
unsigned char calculatedChecksum;

void UartDecodeMessage(unsigned char c) {
    switch (rcvState) {
        case WAITING:
            if (c == 0xFE) {
                rcvState = FUNCTIONMSB;
            }
            break;

        case FUNCTIONMSB:
            msgDecodedFunction = (c << 8);
            rcvState = FUNCTIONLSB;
            break;

        case FUNCTIONLSB:
            msgDecodedFunction += c;
            rcvState = PAYLOADLENGTHMSB;
            break;

        case PAYLOADLENGTHMSB:
            msgDecodedPayloadLength = (c << 8);
            rcvState = PAYLOADLENGTHLSB;
            break;

        case PAYLOADLENGTHLSB:
            msgDecodedPayloadLength += c;
            if (msgDecodedPayloadLength == 0) {
                rcvState = CHECKSUM;
            } else {
                msgDecodedPayloadIndex = 0;
                rcvState = PAYLOAD;
            }
            break;

        case PAYLOAD:
            if (msgDecodedPayloadIndex < msgDecodedPayloadLength) {
                msgDecodedPayload[msgDecodedPayloadIndex] = c;
                msgDecodedPayloadIndex++;
                if (msgDecodedPayloadIndex == msgDecodedPayloadLength) {
                    rcvState = CHECKSUM;
                }
            }
            break;

        case CHECKSUM:

            receivedChecksum = c;
            calculatedChecksum = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            if (calculatedChecksum == receivedChecksum) {
                UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            }
            rcvState = WAITING;
            break;

        default:
            rcvState = WAITING;
            break;
    }

}

int msgFunction;

void UartProcessDecodedMessage(int function,
        int payloadLength, unsigned char* payload) {
    switch (function) {
        case SET_ROBOT_STATE:
            SetRobotState(payload[0]);
            break;

        case SET_ROBOT_MANUAL_CONTROL:
            SetRobotAutoControlState(payload[0]);
            break;

        default:
            break;

    }
}
//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/


void SetRobotAutoControlState(unsigned char payload){
    if(payload!=autoControlActivated){
        autoControlActivated=payload;
    }
}