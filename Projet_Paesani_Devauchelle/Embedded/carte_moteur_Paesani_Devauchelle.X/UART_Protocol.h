#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload);
void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload);
void UartDecodeMessage(unsigned char c);
void UartProcessDecodedMessage(int function, int payloadLength, unsigned char* payload);

void SetRobotState(unsigned char payload);
void SetRobotAutoControlState(unsigned char payload);

#define WAITING 0
#define FUNCTIONMSB 1
#define FUNCTIONLSB 2
#define PAYLOADLENGTHMSB 3
#define PAYLOADLENGTHLSB 4
#define PAYLOAD 5
#define CHECKSUM 6

#define SET_ROBOT_STATE 0x0051
#define SET_ROBOT_MANUAL_CONTROL 0x0052

extern unsigned char autoControlActivated = 1;

#endif	/* UART_PROTOCOL_H */

