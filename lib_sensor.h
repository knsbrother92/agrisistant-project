#ifndef  _LIB_SENSOR_
#define  _LIB_SENSOR_

#include "stm32f10x_lib.h"
//#include"lcd.h"

//===================================================================
//
// adc_sht11.h (@chlee)
//
//===================================================================
// Copyright 2006, HANBACK
//===================================================================
// 
// Author(s): Cheol-Hee,Lee
// Date     : 2005-01-6
// Purpose  : ZigbeX definitions for sht11 of qplusn
// Usage    : This file is used in nano qplus OS
//
// Author(s): Jee Hwn Song (Converter)
// Date     : 2008/07/25
// Purpose  : STM32F10x 
// Usage    : STH11 Library
//        
//===================================================================


void sht11_external_isr(void);
void start_sht11_sensor(void);
u16 get_sht11_hanback_data(u8 type);

u8 s_write_byte(u8 value);
u8 s_read_byte(u8 ack);
void s_transstart(void);
void s_connectionreset(void);
u8 s_softreset(void);
u8 s_read_statusreg(u8 *p_value, u8 *p_checksum);
u8 s_write_statusreg(u8 *p_value);
u8 s_measure(u16 *p_value, u16 *p_checksum, u8 mode);
void calc_sth11(u16 p_humidity ,u16 p_temperature);
float calc_dewpoint(float h,float t);
void initialize_sht11_hanback(void);
void sht11_delay(unsigned short time_us);


enum {DEW,TEMP,HUMI};


#endif 
