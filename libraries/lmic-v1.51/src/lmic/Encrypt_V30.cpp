/******************************************************************************************
* Copyright 2015, 2016 Ideetron B.V.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************************/

/******************************************************************************************
*
* File:        Encrypt_V30.cpp
* Author:      Gerben den Hartog (Ideetron B.V.). 
*
* Ported to the regular Arduino platform running the LMIC-1.5 stack by Maarten Westenberg.
*
*
******************************************************************************************/
/****************************************************************************************
*
* Created on: 			05-11-2015
* Supported Hardware: ID150119-02 Nexus board with RFM95
*
* Firmware Version 1.0
* First version
*
* Firmware Version 2.0
* Works the same is 1.0 using own AES encryption
*
* Firmware Version 3.0
* Included direction in MIC calculation and encryption
****************************************************************************************/

#define TEST 0
#if TEST==1
#include <Arduino.h>
#endif
/*
*****************************************************************************************
* INCLUDE FILES
*****************************************************************************************
*/

#include "lmic.h"
#include "Encrypt_V30.h"
#include "AES-128_V10.h"

/*
*****************************************************************************************
* INCLUDE GLOBAL VARIABLES
*****************************************************************************************
*/


// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
void Encrypt_Payload(unsigned char *Data, unsigned char Data_Length, unsigned int Frame_Counter, unsigned char Direction)
{
	unsigned char i = 0x00;
	unsigned char j;
	unsigned char Number_of_Blocks = 0x00;
	unsigned char Incomplete_Block_Size = 0x00;

	unsigned char Block_A[16];

	//Calculate number of blocks
	Number_of_Blocks = Data_Length / 16;
	Incomplete_Block_Size = Data_Length % 16;
	if(Incomplete_Block_Size != 0)
	{
		Number_of_Blocks++;
	}

	for(i = 1; i <= Number_of_Blocks; i++)
	{
		Block_A[0] = 0x01;
		Block_A[1] = 0x00;
		Block_A[2] = 0x00;
		Block_A[3] = 0x00;
		Block_A[4] = 0x00;

		Block_A[5] = Direction;
		os_wlsbf4 (Block_A+6, LMIC.devaddr);	// XXX 160508 Use LMIC value
		//Block_A[6] = DevAddr[3];	// XXX 160508 was DevAddr[3]
		//Block_A[7] = DevAddr[2];
		//Block_A[8] = DevAddr[1];
		//Block_A[9] = DevAddr[0];

		Block_A[10] = (Frame_Counter & 0x00FF);
		Block_A[11] = ((Frame_Counter >> 8) & 0x00FF);

		Block_A[12] = 0x00; //Frame counter upper Bytes
		Block_A[13] = 0x00;

		Block_A[14] = 0x00;

		Block_A[15] = i;

#if TEST==1
	Serial.print("artKey "); for (i=0; i<16; i++) { if (LMIC.artKey[i]<16) Serial.print('0'); Serial.print(LMIC.artKey[i],HEX); }; Serial.println();
#endif
		//Calculate S
		//AES_Encrypt(Block_A, AppSkey);					// XXX 160510; AppSkey is filled AFTER Join Accepted
		AES_Encrypt(Block_A, LMIC.artKey);					// XXX 160510; AppSkey is filled AFTER Join Accepted
		
		//Check for last block
		if(i != Number_of_Blocks)
		{
			for(j = 0; j < 16; j++)
			{
				*Data = *Data ^ Block_A[j];
				Data++;
			}
		}
		else
		{
			if(Incomplete_Block_Size == 0)
			{
				Incomplete_Block_Size = 16;
			}
			for(j = 0; j < Incomplete_Block_Size; j++)
			{
				*Data = *Data ^ Block_A[j];
				Data++;
			}
		}
	}
}

// --------------------------------------------------------------------
// Calculate a MIC (Not for Join Request MIC0)
// --------------------------------------------------------------------
void Calculate_MIC(unsigned char *Data, unsigned char *Final_MIC, unsigned char Data_Length, unsigned int Frame_Counter, unsigned char Direction)
{
	unsigned char i;
	unsigned char Key_K1[16] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	unsigned char Key_K2[16] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	unsigned char Old_Data[16] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	unsigned char New_Data[16] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	unsigned char Block_B[16];
	unsigned char Number_of_Blocks = 0x00;
	unsigned char Incomplete_Block_Size = 0x00;
	unsigned char Block_Counter = 0x01;

#if TEST==1
	Serial.print("AESkey "); for (i=0; i<16; i++) { if (AESkey[i]<16) Serial.print('0'); Serial.print(AESkey[i],HEX); }; Serial.println();
	Serial.print("devadd "); Serial.print(LMIC.devaddr,HEX); Serial.println();
#endif
	
  // Assume that for framecounter==0 we are starting all over and we do not want AESAUX
  if (Frame_Counter != 0) 
  {

	//Create Block_B
	Block_B[0] = 0x49;
	Block_B[1] = 0x00;
	Block_B[2] = 0x00;
	Block_B[3] = 0x00;
	Block_B[4] = 0x00;

	Block_B[5] = Direction;

	os_wlsbf4 (Block_B+6, LMIC.devaddr);	// XXX 160508 Use LMIC value
	//Block_B[6] = DevAddr[3];				// XXX 160508 Changed from DevAddr[3] taken from sketch
	//Block_B[7] = DevAddr[2];
	//Block_B[8] = DevAddr[1];
	//Block_B[9] = DevAddr[0];

	Block_B[10] = (Frame_Counter & 0x00FF);
	Block_B[11] = ((Frame_Counter >> 8) & 0x00FF);

	Block_B[12] = 0x00; //Frame counter upper bytes
	Block_B[13] = 0x00;

	Block_B[14] = 0x00;
	Block_B[15] = Data_Length;
  }
	//Calculate number of Blocks and blocksize of last block
	Number_of_Blocks = Data_Length / 16;
	Incomplete_Block_Size = Data_Length % 16;

	if(Incomplete_Block_Size != 0)
	{
		Number_of_Blocks++;
	}

  if (Frame_Counter == 0) {
	  Generate_Keys(Key_K1, Key_K2, true);
  }
  else {
	Generate_Keys(Key_K1, Key_K2, false);
	//Preform Calculation on Block B0

	//Perform AES encryption
	AES_Encrypt(Block_B,AESkey);						// XXX 1605009; NwkSkey is changed as soon as JOINED	

	//Copy Block_B to Old_Data
	for(i = 0; i < 16; i++)
	{
		Old_Data[i] = Block_B[i];
	}
  }

	//Preform full calculating until n-1 messsage blocks
	while(Block_Counter < Number_of_Blocks)
	{
		//Copy data into array
		for(i = 0; i < 16; i++)
		{
			New_Data[i] = *Data;
			Data++;
		}

		//Preform XOR with old data
		XOR(New_Data,Old_Data);

		//Preform AES encryption
  //if (Frame_Counter == 0)
		AES_Encrypt(New_Data,AESkey);							// XXX 1605009; NwkSkey is changed as soon as JOINED
  //else
	//	AES_Encrypt(New_Data,LMIC.nwkKey);						// XXX 1605009; NwkSkey is changed as soon as JOINED

		//Copy New_Data to Old_Data
		for(i = 0; i < 16; i++)
		{
			Old_Data[i] = New_Data[i];
		}

		//Raise Block counter
		Block_Counter++;
	}

	//Perform calculation on last block
	//Check if Datalength is a multiple of 16
	if(Incomplete_Block_Size == 0)
	{
		//Copy last data into array
		for(i = 0; i < 16; i++)
		{
			New_Data[i] = *Data;
			Data++;
		}

		//Preform XOR with Key 1
		XOR(New_Data,Key_K1);

		//Preform XOR with old data
		XOR(New_Data,Old_Data);
	  //if (Frame_Counter==0)
		//Preform last AES routine
		AES_Encrypt(New_Data,AESkey);
	  //else
		//AES_Encrypt(New_Data,LMIC.nwkKey);
	}
	else
	{
		//Copy the remaining data and fill the rest
		for(i = 0; i < 16; i++)
		{
			if(i < Incomplete_Block_Size)
			{
				New_Data[i] = *Data;
				Data++;
			}
			if(i == Incomplete_Block_Size)
			{
				New_Data[i] = 0x80;
			}
			if(i > Incomplete_Block_Size)
			{
				New_Data[i] = 0x00;
			}
		}

		//Preform XOR with Key 2
		XOR(New_Data,Key_K2);

		//Preform XOR with Old data
		XOR(New_Data,Old_Data);
	  //if (Frame_Counter==0) 
		  AES_Encrypt(New_Data,AESkey);	//Preform last AES routine
	  //else 
		//  AES_Encrypt(New_Data,LMIC.nwkKey);
	}

	Final_MIC[0] = New_Data[0];
	Final_MIC[1] = New_Data[1];
	Final_MIC[2] = New_Data[2];
	Final_MIC[3] = New_Data[3];
}


// --------------------------------------------------------------------
// Generate some keys
// --------------------------------------------------------------------
void Generate_Keys(unsigned char *K1, unsigned char *K2, bool joining)
{
	unsigned char i;
	unsigned char MSB_Key;

	//Encrypt the zeros in K1 with the AESkey
	//Except when join is in progress
	if (joining) 
		AES_Encrypt(K1,AESkey);		// XXX 160509
	else 
		AES_Encrypt(K1,LMIC.nwkKey);	// XXX 160508
	
	//Create K1
	//Check if MSB is 1
	if((K1[0] & 0x80) == 0x80)
	{
		MSB_Key = 1;
	}
	else
	{
		MSB_Key = 0;
	}

	//Shift K1 one bit left
	Shift_Left(K1);

	//if MSB was 1
	if(MSB_Key == 1)
	{
		K1[15] = K1[15] ^ 0x87;
	}

	//Copy K1 to K2
	for( i = 0; i < 16; i++)
	{
		K2[i] = K1[i];
	}

	//Check if MSB is 1
	if((K2[0] & 0x80) == 0x80)
	{
		MSB_Key = 1;
	}
	else
	{
		MSB_Key = 0;
	}

	//Shift K2 one bit left
	Shift_Left(K2);

	//Check if MSB was 1
	if(MSB_Key == 1)
	{
		K2[15] = K2[15] ^ 0x87;
	}
}

// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
void Shift_Left(unsigned char *Data)
{
	unsigned char i;
	unsigned char Overflow = 0;
	//unsigned char High_Byte, Low_Byte;

	for(i = 0; i < 16; i++)
	{
		//Check for overflow on next byte except for the last byte
		if(i < 15)
		{
			//Check if upper bit is one
			if((Data[i+1] & 0x80) == 0x80)
			{
				Overflow = 1;
			}
			else
			{
				Overflow = 0;
			}
		}
		else
		{
			Overflow = 0;
		}

		//Shift one left
		Data[i] = (Data[i] << 1) + Overflow;
	}
}

// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
void XOR(unsigned char *New_Data,unsigned char *Old_Data)
{
	unsigned char i;

	for(i = 0; i < 16; i++)
	{
		New_Data[i] = New_Data[i] ^ Old_Data[i];
	}
}

