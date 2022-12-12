#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload);
void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload);
void UartDecodeMessage(unsigned char c);
void UartProcessDecodedMessage(int function, int payloadLength, unsigned char* payload);

#define WAITING 0
#define FUNCTIONMSB 1
#define FUNCTIONLSB 2
#define PAYLOADLENGTHMSB 3
#define PAYLOADLENGTHLSB 4
#define PAYLOAD 5
#define CHECKSUM 6

#endif	/* UART_PROTOCOL_H */

