/******************************************************************************************
*
* File:        Encrypt_V30.h
* Author:      Gerben den Hartog
* Compagny:    Ideetron B.V.
* Website:     http://www.ideetron.nl/LoRa
* E-mail:      info@ideetron.nl
******************************************************************************************/
/****************************************************************************************
*
* Created on: 			05-11-2015
* Supported Hardware: ID150119-02 Nexus board with RFM95
*
* Firmware Version 2.0
* First version
*
* Firmware Version 2.0
* Works the same is 1.0 using own AES encryption
*
* Firmware Version 3.0
* Included direction in MIC calculation and encryption
****************************************************************************************/

#ifndef ENCRYPT_V30_H
#define ENCRYPT_V30_H

/*
*****************************************************************************************
* FUNCTION PROTOTYPES
*****************************************************************************************
*/

void Calculate_MIC(unsigned char *Data, unsigned char *Final_MIC, unsigned char Data_Length, unsigned int Frame_Counter, unsigned char Direction);
void Encrypt_Payload(unsigned char *Data, unsigned char Data_Length, unsigned int Frame_Counter, unsigned char Direction);
void Generate_Keys(unsigned char *K1, unsigned char *K2);
void Shift_Left(unsigned char *Data);
void XOR(unsigned char *New_Data,unsigned char *Old_Data);

#endif
