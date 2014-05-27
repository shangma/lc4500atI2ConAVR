/*
 * runAvrOnArduino.c
 *
 * Created: 5/14/2014 5:46:09 PM
 *  Author: ms
 */ 



/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* 9600 baud */
#define UART_BAUD_RATE 9600
#define ERROR 0
#define SUCCESS (!ERROR)

#include <stdlib.h>
#include <avr/io.h>
#include <string.h>     /* strcat */
#include <stdlib.h>     /* strtol */
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "uart.h"
#include "twi.h"
#include "dlpstatus.h"


uint8_t DLPCReadByte(uint8_t readCommand, uint8_t bytesOfCommandData, uint8_t* readResult);
uint8_t DLPWriteByte(uint8_t writeCommand, uint8_t bytesOfCommandData, uint8_t* writeData);

int  DLPPatternDisplayModeSetup();


const char *byte_to_binary(int x)
{
	static char b[9];
	b[0] = '\0';

	int z;
	for (z = 128; z > 0; z >>= 1)
	{
		strcat(b, ((x & z) == z) ? "1" : "0");
	}

	return b;
}


int main(void)
{
	uint8_t u8ReadStatus;
	uint8_t u8WriteStatus;
	
	uint8_t u8WriteBuffer[16];
	
	char getCommand;
	
	uint8_t displayModeReadResult;
	uint8_t displayModeControlReadResult;
	
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU) );
    sei();
	TWIInit();

	_delay_ms(1000);
			
    while(1)
    {
        //TODO:: Please write your application code 	
		
		getCommand = uart_getc();
		
		if (getCommand & UART_NO_DATA)
		{
		}
		else 
		{
			if (getCommand == 'm')
			{
				uart_puts("read display mode");
				u8ReadStatus = DLPCReadByte(displayModeRead, 1, &displayModeReadResult);
				if (u8ReadStatus == SUCCESS)
				{
					uart_puts("Display mode: ");
					uart_puts(byte_to_binary(displayModeReadResult));
					uart_puts("\r\n");
					
					if (displayModeReadResult == 0x00)
					{
						u8WriteBuffer[0] = 0x01;
						u8WriteStatus = DLPWriteByte(displayModeWrite, 1, &u8WriteBuffer[0]);
						if (u8WriteStatus == SUCCESS)
						{
							uart_puts("Write display mode 0x01");
							uart_puts("\r\n");
						}
					} else if (displayModeReadResult == 0x01)
					{
						u8WriteBuffer[0] = 0x00;
						u8WriteStatus = DLPWriteByte(displayModeWrite, 1, &u8WriteBuffer[0]);
						if (u8WriteStatus == SUCCESS)
						{
							uart_puts("Write display mode 0x00");
							uart_puts("\r\n");
						}
					}	
				} else {
					uart_puts("Read Mode Error");
					uart_puts("\r\n");
				}
				
				uint8_t hardwareStatusReadResult;
				u8ReadStatus = DLPCReadByte(hardwareStatusReadOnly, 1, &hardwareStatusReadResult);
				if (u8ReadStatus == SUCCESS)
				{
					uart_puts("Hardware status: ");
					uart_puts(byte_to_binary(hardwareStatusReadResult));
					uart_puts("\r\n");
				} else {
					uart_puts("Read hardware status Error");
					uart_puts("\r\n");
				}
				
			} 
			else if (getCommand == 'p')
			{
				uart_puts("read display mode");
				uart_puts("\r\n");
				u8ReadStatus = DLPCReadByte(displayModeRead, 1, &displayModeReadResult);
				if (u8ReadStatus == SUCCESS)
				{
					if (displayModeReadResult == 0x01)
					{
						uart_puts("read display control");
						u8ReadStatus = DLPCReadByte(displayModeControlRead, 1, &displayModeControlReadResult);
						if (u8ReadStatus == SUCCESS)
						{
							uart_puts("Display mode control: ");
							uart_puts(byte_to_binary(displayModeControlReadResult));
							uart_puts("\r\n");
							
							if (displayModeControlReadResult == 0b00000010)
							{
								u8WriteBuffer[0] = 0b00000001;
								u8WriteStatus = DLPWriteByte(displayModeControlWrite, 1, &u8WriteBuffer[0]);
								if (u8WriteStatus == SUCCESS)
								{
									uart_puts("Write display control 0x01");
									uart_puts("\r\n");
								}
							} 
							else if (displayModeControlReadResult == 0b00000001)
							{
								u8WriteBuffer[0] = 0b00000010;
								u8WriteStatus = DLPWriteByte(displayModeControlWrite, 1, &u8WriteBuffer[0]);
								if (u8WriteStatus == SUCCESS)
								{
									uart_puts("Write display control 0x02");
									uart_puts("\r\n");
								}
							}
							else if (displayModeControlReadResult == 0b00000000)
							{
								u8WriteBuffer[0] = 0b00000010;
								u8WriteStatus = DLPWriteByte(displayModeControlWrite, 1, &u8WriteBuffer[0]);
								if (u8WriteStatus == SUCCESS)
								{
									uart_puts("Write display control 0x02");
									uart_puts("\r\n");
								}
							}
						} else {
							uart_puts("Read Mode Control Error");
							uart_puts("\r\n");
						}
					}
				} else {
				uart_puts("Read Mode Error");
				uart_puts("\r\n");
				}	
								
			} else if (getCommand == 's')
			{
				uart_puts("read display mode \r\n");
				u8ReadStatus = DLPCReadByte(displayModeRead, 1, &displayModeReadResult);
				if (u8ReadStatus == SUCCESS)
				{
					if (displayModeReadResult == 0x01)
					{
						uart_puts("read display control");
						u8ReadStatus = DLPCReadByte(displayModeControlRead, 1, &displayModeControlReadResult);
						if (u8ReadStatus == SUCCESS)
						{
							uart_puts("Display mode control: ");
							uart_puts(byte_to_binary(displayModeControlReadResult));
							uart_puts("\r\n");
							
							if (displayModeControlReadResult == 0b00000000)
							{
								u8WriteBuffer[0] = 0b00000010;
								u8WriteStatus = DLPWriteByte(displayModeControlWrite, 1, &u8WriteBuffer[0]);
								if (u8WriteStatus == SUCCESS)
								{
									uart_puts("Write display control 0x02");
									uart_puts("\r\n");
								}
							}
							else if (displayModeControlReadResult == 0b00000010)
							{
								u8WriteBuffer[0] = 0b00000000;
								u8WriteStatus = DLPWriteByte(displayModeControlWrite, 1, &u8WriteBuffer[0]);
								if (u8WriteStatus == SUCCESS)
								{
									uart_puts("Write display control 0x00");
									uart_puts("\r\n");
								}
							}
						}
					}
				}
			} else if (getCommand == 't')
			{
				int result = DLPPatternDisplayModeSetup();
				if (result == 0)
				{
					uart_puts("Pattern display mode setup SUCCESS");
					uart_puts("\r\n");
				}
			}
		}
    }
}


int  DLPPatternDisplayModeSetup()
{
	uint8_t u8ReadStatus;
	uint8_t u8WriteStatus;
	uint8_t u8WriteBuffer[10];
	
	// 1. Enable pattern display mode
	u8WriteBuffer[0] = 0x01;
	u8WriteStatus = DLPWriteByte(displayModeWrite, 1, &u8WriteBuffer[0]); // 0x69
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("enable pattern display");
		uart_puts("1\r\n");
	}
	
		
	// 2. Set pattern display mode to flash image or external video
	u8WriteBuffer[0] = 0x03;
	u8WriteStatus = DLPWriteByte(displayDataInputSourceWrite, 1, &u8WriteBuffer[0]); // 0x6F
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("Write data input source 0x03");
		uart_puts("2\r\n");
	}
	_delay_ms(100); // needed, don't remove this line

	// 3.0. Ensure pattern sequence has been stopped
	u8WriteBuffer[0] = 0x00;
	u8WriteStatus = DLPWriteByte(displayModeControlWrite, 1, &u8WriteBuffer[0]); // 0x65
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("Stop the pattern sequence");
		uart_puts("3\r\n");
	}
	
	// 3.1. set the number of patterns
	
	u8WriteBuffer[0] = 0x01; // 0x17 = 23; value+1 as number of LUT entries
	u8WriteBuffer[1] = 0x01; // pattern repeat
	u8WriteBuffer[2] = 0x01; // 0x17 = 23; value+1 as number of patterns, irrelevant if repeated
	u8WriteBuffer[3] = 0x00; // 0x00 = 0; value+1 as number of splash images used
	
	u8WriteStatus = DLPWriteByte(patternDisplayLUTControlWrite, 4, &u8WriteBuffer[0]); // 0x75
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("set the number of patterns");
		uart_puts("3.1\r\n");
	}
	
	
	// 4. set the pattern trigger mode
	u8WriteBuffer[0] = 0x01; // internal trigger
	u8WriteStatus = DLPWriteByte(patternTriggerModeWrite, 1, &u8WriteBuffer[0]); // 0x70
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("set the pattern trigger mode");
		uart_puts("4\r\n");
	}
		
	// 5. set the exposure and frame rate (four bytes for each)
	
	// for 100000us
	u8WriteBuffer[0] = 0x00; //Exposure time LSB + 3
	u8WriteBuffer[1] = 0x01; //Exposure time LSB + 2
	u8WriteBuffer[2] = 0x86; //Exposure time LSB + 1
	u8WriteBuffer[3] = 0xA0; //Exposure time LSB
	
	// for 100000us
	u8WriteBuffer[4] = 0x00; //Frame time LSB + 3
	u8WriteBuffer[5] = 0x01; //Frame time LSB + 2
	u8WriteBuffer[6] = 0x86; //Frame time LSB + 1
	u8WriteBuffer[7] = 0xA0; //Frame time LSB
	

	u8WriteStatus = DLPWriteByte(patternExposureTimeAndFrameWrite, 8, &u8WriteBuffer[0]); // 0x66
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("set exposure and frame rate");
		uart_puts("5\r\n");
	}
	
	// 6. set up the image indexes for using flash images
	
	// 6.a. open the mailbox for image indexes configuration
	u8WriteBuffer[0] = 0x01;
	u8WriteStatus = DLPWriteByte(patternDisplayLUTAccessWriteOnly, 1, &u8WriteBuffer[0]); // 0x77
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("open the mailbox for image index");
		uart_puts("6.a\r\n");
	}
		
	// 6.b. set mailbox offset
	u8WriteBuffer[0] = 0x00;
	u8WriteStatus = DLPWriteByte(patternDisplayMailboxOffsetWriteOnly, 1, &u8WriteBuffer[0]); // 0x76
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("set the mailbox offset");
		uart_puts("b\r\n");
	}

		
	// 6.c. set image indexes
	u8WriteBuffer[0] = 0x08;
	u8WriteStatus = DLPWriteByte(patternDisplayImageIndexWriteOnly, 1, &u8WriteBuffer[0]); // 0x78
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("set image indexes");
		uart_puts("c\r\n");
	}
			
	// 6.d. close the mailbox
	u8WriteBuffer[0] = 0x00;
	u8WriteStatus = DLPWriteByte(patternDisplayLUTAccessWriteOnly, 1, &u8WriteBuffer[0]); // 0x77
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("close the mailbox for image index");
		uart_puts("d\r\n");
	}
		
	// 7. set up the LUT
	// 7.a open mailbox for pattern definition 
	u8WriteBuffer[0] = 0x02;
	u8WriteStatus = DLPWriteByte(patternDisplayLUTAccessWriteOnly, 1, &u8WriteBuffer[0]); // 0x77
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("open the mailbox for pattern");
		uart_puts("7.a\r\n");
	}
		
	// 7.b set mailbox offset 
	u8WriteBuffer[0] = 0x00;
	u8WriteStatus = DLPWriteByte(patternDisplayMailboxOffsetWriteOnly, 1, &u8WriteBuffer[0]); // 0x76
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("set the mailbox offset");
		uart_puts("b\r\n");
	}
	
	
	// 7.c fill pattern sequence data 
	// (i) Byte 0, b1:0- choose trigger: internal (0x00), external positive (0x01), external negative (0x02),
	//		continue from previous (0x03)
	//	(ii) Byte 0, b7:2- choose pattern number (what bit planes will be illuminated). Max is 24 for 1 bitdepth
	//	(iii) Byte 1, b3:0, choose bit weight (1-8)
	//	(iv) Byte 1, b6:4, choose which LEDs are on (Blue, Green, Red)
	//	(v) Byte 2
	//		(i) b0- invert pattern if 1
	//		(ii) b1- insert black pattern after current pattern if 1 (should be 0 if continuous trigger)
	//		(iii) b2- perform buffer swap if 1
	//		(iv) b3- trigger out1 stays high (if this stays high for n patterns, then exposure time is shared
	//				between n patterns)
	//	(vi) Repeat (I) through (v) for each pattern in the sequence
	
	// pattern 1
	u8WriteBuffer[0] = 0x00;
	u8WriteBuffer[1] = 0x21;
	u8WriteBuffer[2] = 0x06;
	// pattern 2
	u8WriteBuffer[3] = 0x04;
	u8WriteBuffer[4] = 0x21;
	u8WriteBuffer[5] = 0x00;	
	
	// 0x78
	u8WriteStatus = DLPWriteByte(patternDisplayImageIndexWriteOnly, 6, &u8WriteBuffer[0]); // 0x78
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("fill pattern data");
		uart_puts("c\r\n");
	}	
		
	// 7.d close mailbox
	u8WriteBuffer[0] = 0x00;
	u8WriteStatus = DLPWriteByte(patternDisplayLUTAccessWriteOnly, 1, &u8WriteBuffer[0]);  // 0x77
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("close the mailbox for pattern");
		uart_puts("d\r\n");
	}
	
	
	// 7.e  write dummy data for validation
	u8WriteBuffer[0] = 0x00;
	u8WriteStatus = DLPWriteByte(patternDisplayValidationWrite, 1, &u8WriteBuffer[0]); // 0x7D
	if (u8WriteStatus == SUCCESS)
	{
		//uart_puts("write dummy data for validation");
		uart_puts("e\r\n");
	}
	
	_delay_ms(1000); // needed, don't remove this line, don't change the time
	// 8. Execute the validation command
	
	uint8_t validationReadResult;
	u8ReadStatus = DLPCReadByte(patternDisplayValidationRead, 1, &validationReadResult); // 0x7D for read
	if (u8ReadStatus == SUCCESS)
	{
		uart_puts("8 read: ");
		uart_puts(byte_to_binary(validationReadResult));
		uart_puts(" \r\n");
		if (validationReadResult != 0x00)
		{
			//return -1;
			uart_puts("read dummy error");
		} else {
			uart_puts("8");
			uart_puts("\r\n");
		}
	}	
	
	// 9. Read statuses
	
	uint8_t hardwareStatusReadResult;
	u8ReadStatus = DLPCReadByte(hardwareStatusReadOnly, 1, &hardwareStatusReadResult); // 0x20
	if (u8ReadStatus == SUCCESS)
	{
		uart_puts("Hardware status: ");
		uart_puts(byte_to_binary(hardwareStatusReadResult));
		uart_puts("\r\n");
	}
	uint8_t systemStatusReadResult;
	u8ReadStatus = DLPCReadByte(systemStatusReadOnly, 1, &systemStatusReadResult); // 0x21
	if (u8ReadStatus == SUCCESS)
	{
		uart_puts("System status: ");
		uart_puts(byte_to_binary(systemStatusReadResult));
		uart_puts("\r\n");
	}
	uint8_t mainStatusReadResult;
	u8ReadStatus = DLPCReadByte(mainStatusReadOnly, 1, &mainStatusReadResult); // 0x22
	if (u8ReadStatus == SUCCESS)
	{
		uart_puts("Main status: ");
		uart_puts(byte_to_binary(mainStatusReadResult));
		uart_puts("\r\n");
	}
	
	
	_delay_ms(1000); // needed, don't remove this line
	
	// 10. start display patterns
	u8WriteBuffer[0] = 0x02;
	u8WriteStatus = DLPWriteByte(displayModeControlWrite, 1, &u8WriteBuffer[0]); // 0x65
	if (u8WriteStatus == SUCCESS)
	{
		uart_puts("10");
		uart_puts("\r\n");
	}
	
	uart_puts("setup done");
	return 0;
		
}


uint8_t DLPWriteByte(uint8_t writeCommand, uint8_t bytesOfCommandData, uint8_t*  writeData)
{
//	uart_puts("write will start \r\n");
	
	TWIStart();
	if (TWIGetStatus() != 0x08)
	{
//		uart_puts("Can't start: ");
//		uart_putc(TWIGetStatus());
		return ERROR;
	}
//	uart_puts("write starts \r\n");
	
	TWIWrite(writeOnLow);
	if (TWIGetStatus() != 0x18)
	{
//		uart_puts("Can't write: ");
//		uart_putc(TWIGetStatus());
		return ERROR;
	}
//	uart_puts("write address \r\n");
	
	TWIWrite(writeCommand);
	if (TWIGetStatus() != 0x28)
	{
//		uart_puts("Can't write: ");
//		uart_putc(TWIGetStatus());
		return ERROR;
	}
	
	
	for (int i=0; i<bytesOfCommandData; i++)
	{
		TWIWrite(*writeData++);
		if (TWIGetStatus() != 0x28)
		{
//			uart_puts("Can't write: ");
//			uart_putc(TWIGetStatus());
			return ERROR;
		}
	}
	
	/*
	TWIWrite(writeData);	
	*/
	
	TWIStop();
//	uart_puts("write end \r\n");
	return SUCCESS;
	
}

uint8_t DLPCReadByte(uint8_t readCommand, uint8_t bytesOfCommandData, uint8_t * readResult)
{
	uart_puts("-- read will start \r\n");
	
	TWIStart();
	if (TWIGetStatus() == 0x08)
	{
		uart_puts("---- read starts \r\n");
	} else if (TWIGetStatus() == 0x10)
	{
		uart_puts("---- start again \r\n");	
	} else
	{
		uart_puts("---- Can't start: ");
		uart_putc(TWIGetStatus());
		return ERROR;
	}

	
	TWIWrite(writeOnLow);
	if (TWIGetStatus() != 0x18)
	{
		uart_puts("---- Can't write: ");
		uart_putc(TWIGetStatus());
		return ERROR;
	}
	uart_puts("---- write address \r\n");
	
	TWIWrite(readCommand);
	if (TWIGetStatus() != 0x28)
	{
		uart_puts("---- Can't write: ");
		uart_putc(TWIGetStatus());
		return ERROR;
	}
	uart_puts("---- write command \r\n");
	
	TWIStart();
	if (TWIGetStatus() != 0x10)
	{
		uart_puts("---- Can't start: ");
		uart_putc(TWIGetStatus());
		return ERROR;
	}
	uart_puts("---- read starts again\r\n");
	
	TWIWrite(readOnLow);
	if (TWIGetStatus() != 0x40)
	{
		uart_puts("---- Can't write: ");
		uart_putc(TWIGetStatus());
		return ERROR;
	}
	uart_puts("---- write readOnLow\r\n");
	
	uint8_t readStatus = TWIReadACK();
	if (TWIGetStatus() != 0x50)
	{
		uart_puts("---- Can't read: ");
		uart_putc(TWIGetStatus());
		return ERROR;
	}
	uart_puts("---- get read status\r\n");
	
	while (!readStatus)				
	{
		readStatus = TWIReadACK();
	}
	
	if (readStatus == 0x01)
	{
		uart_puts("---- right status\r\n");
	} else {
		uart_puts("---- wrong status \r\n"); // shouldn't come into here
	}
	
	for (int i=0; i<bytesOfCommandData; i++)
	{
		*readResult++ = TWIReadACK();
		if (TWIGetStatus() != 0x50)
		{
			uart_puts("---- Can't read: ");
			uart_putc(TWIGetStatus());
			return ERROR;
		}
	}
	
	TWIStop();
	uart_puts("-- read end \r\n");
	return SUCCESS;
}

