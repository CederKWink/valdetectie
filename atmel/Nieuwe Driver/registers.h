#ifndef _REGISTERS_HPP
#define _REGISTERS_HPP

  //key bit locations
  //rw bit 1 = r & 0 = w
  #define RW_bp  7
  #define RW_bm  0x80
  //multiple read bit 1 = increment every read action
  #define MS_bp  6
  #define MS_bm  0x40
	
	#define ZYXDA_bm		0x08
	
	//In  =   INTn pin
	//IAn = 	interrupt function n
  
  //------------------------------------------//
	//register addresses for the accelrometer
	//------------------------------------------//

	// dummy address
  #define WHO_AM_I_REG 0x0F
  
  //data rate & power mode register & axes enable
  #define CTRL_REG1  0x20
  //high pass filter configurations
  #define CTRL_REG2  0x21
	//INT1 pin trigger source configurations
  #define CTRL_REG3  0x22
  //data output configuration register
  #define CTRL_REG4  0x23
  //set interrupt settings
  #define CTRL_REG5  0x24
  //INT2 pin trigger source configurations
  #define CTRL_REG6  0x25
  
	
	//register wich reports certain statusses
  #define STATUS_REG  0x27  
  
	// settings for interrupt funciton 1 
  #define INT1_CFG        0x30
  #define INT1_SRC        0x31
  #define INT1_THS        0x32
  #define INT1_DURATION   0x33
  
  // settings for interrupt funciton 2
  #define INT2_CFG        0x34
  #define INT2_SRC        0x35
  #define INT2_THS        0x36
  #define INT2_DURATION   0x37
  
  
  //ACTIVITY threshold
  #define ACT_THS         0x3E 
  //ACTIVITY duration
  #define ACT_DURATION    0x3F  
  
  

  #define OUT_X_L_REG  0x28
  #define OUT_X_H_REG  0x29
  #define OUT_Y_L_REG  0x2A
  #define OUT_Y_H_REG  0x2B
  #define OUT_Z_L_REG  0x2C
  #define OUT_Z_H_REG  0x2D


  //------------------------------------------//
	//Correct values for the registers
	//------------------------------------------//

	// 400 hz, all axes enabled
  #define CTRL_REG1_VALUE  0x77
	// highpass filter on normal mode
  #define CTRL_REG2_VALUE  0x80
	// turn on IA1 on INT1 ( try I1_ZYXDA to try pin funct. )
  #define CTRL_REG3_VALUE  0x10 
	// fullscale is 2g, high resolution enabled
  #define CTRL_REG4_VALUE  0x08
	// dont know if this works try turning something else
  #define CTRL_REG5_VALUE  0x00
	// INT2 triggerd by ACTIVITY enabled
  #define CTRL_REG6_VALUE  0x0A


// NOTE: currently disabled deu to INT2 is triggerd by ACT not IA1
	// OR combination on the axes, enable interrupt event on Z higher than thes
  #define INT1_CFG_VALUE        0x40
  #define INT1_THS_VALUE        0x4A
  #define INT1_DURATION_VALUE   0x06

	// disabled
  #define INT2_CFG_VALUE        0x00
  #define INT2_THS_VALUE        0x00
  #define INT2_DURATION_VALUE   0x00

// NOTE: if there is a value written to these registers the sleep to wake functionality is automatically turned on
// NOTE: if you cant control which axes triggers the activity ( i think its X<THS || Y<THS || Z<THS )
  #define ACT_THS_VALUE         0x26  
  #define ACT_DURATION_VALUE    0xFF  

  //#define ACT_THS_VALUE         0x00
  //#define ACT_DURATION_VALUE    0x00


#endif //_REGISTERS_HPP
