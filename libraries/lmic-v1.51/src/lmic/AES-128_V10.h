/******************************************************************************************
*
* File:        AES-128_V10.h
* Author:      Gerben den Hartog
* Compagny:    Ideetron B.V.
* Website:     http://www.ideetron.nl/LoRa
* E-mail:      info@ideetron.nl
******************************************************************************************/
/****************************************************************************************
*
* Created on: 			20-10-2015
* Supported Hardware: ID150119-02 Nexus board with RFM95
*
* Firmware Version 1.0
* First version
****************************************************************************************/

#ifndef AES128_V10_H
#define AES128_V10_H

/*
********************************************************************************************
* FUNCTION PORTOTYPES
********************************************************************************************
*/

void AES_Encrypt(unsigned char *Data, unsigned char *Key);
void AES_Add_Round_Key(unsigned char *Round_Key);
unsigned char AES_Sub_Byte(unsigned char Byte);
void AES_Shift_Rows();
void AES_Mix_Collums();
void AES_Calculate_Round_Key(unsigned char Round, unsigned char *Round_Key);
void Send_State();

#endif
