/*
 * md_serial.h
 *
 * Created: 4/24/2013 9:31:11 PM
 *  Author: bakker
 */ 


#ifndef MD_SERIAL_H_
#define MD_SERIAL_H_

#include <stdio.h>

extern FILE gCtrl_IO;

void InitSerial(void);

uint8_t CanRead_Ctrl(void);
uint8_t ReadByte_Ctrl(void);
uint8_t CanWrite_Ctrl(void);
void WriteByte_Ctrl(uint8_t data);

uint8_t CanRead_FC(void);
uint8_t ReadByte_FC(void);
uint8_t CanWrite_FC(void);
void WriteByte_FC(uint8_t data);

#define clear_screen()    fprintf(&gCtrl_IO, "\e[H\e[2J\e[3J");   //!< Macro to reset and clear the terminal

#endif /* MD_SERIAL_H_ */
