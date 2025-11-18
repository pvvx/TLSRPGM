/********************************************************************************************************
 * @file     aoa.h
 *
 * @brief    This is the header file for TLSR8258
 *
 * @author   Driver Group
 * @date     May 8, 2018
 *
 * @par      Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 * @par      History:
 * 			 1.initial release(DEC. 26 2018)
 *
 * @version  A001
 *
 *******************************************************************************************************/

#pragma once

#include "bsp.h"
#include "analog.h"
#include "register.h"
#include "gpio_8258.h"

#define RF_REAL_FREQ			2460
#define IQ_NUM                  45
#define SLOT_TIME				2
#define Lamida                  3*10000*1000/(RF_REAL_FREQ*100 + 25)
//#define sign_mark				1		//for rectangle
#define sign_mark				-1
#define Radius					75
#define XY_TABLE_NUMBER			45
#define Z_TABLE_NUMBER			1

#define SRAM_BOTTOM			    (int *)sram_bottom_aoa	//0x840000
#define SRAM_TOP				(int *)sram_top_aoa   //0x84FFFF

#define sram_bottom_aoa         0x840000
#define sram_top_aoa            0x84FFFF

typedef struct IQ_VALUE
{
	int real_part;
	int imag_part;
}IQ_Value;

typedef struct IQ_CB
{
	IQ_Value IQ[100];
	int number;
}IQ_cb;

typedef struct FUNC_CB
{
	int* 	int_ptr1;
	int* 	int_ptr2;
	char*   char_ptr1;
	char*	char_ptr2;
	int		int1;
	int		int2;
	char	char1;
	char	char2;
	char	char3;
	char	char4;
}Func_CB;

/**
 * @brief      This function servers to get the tan_angle.
 * @param[in]  tan  -  The angel which need to be assignment
 * @return     The tan_angle.
 */
char tan_look_table(unsigned int tan);

/**
 * @brief      This function servers to get the sin_angle.
 * @param[in]  sin  -  The angel which need to be assignment
 * @return     The sin_angle.
 */
char sin_look_table(unsigned int sin);

/**
 * @brief      This function servers to get the cos_angle.
 * @param[in]  cos_val  -  The angel which need to be assignment
 * @return     The cos_angle.
 */
int cos_calculate(int cos_val);

/**
 * @brief      This function servers to demodulation the input angle.
 * @param[in]  real_ptr  -  Pointer variable of real part.
 * @param[in]  imag_ptr  -  Pointer variable of imag part.
 * @param[in]  num       -  The value of the total parts.
 * @return     none.
 */
void demodulation(int* real_ptr,int* imag_ptr,unsigned char num);

//void conj(IQ_cb iq,IQ_cb conj_iq,unsigned char number);

/**
 * @brief      This function servers to calculate the phase.
 * @param[in]  real  -  Stand for the real parts.
 * @param[in]  imag  -  Stand for the image parts.
 * @return     The phase value.
 */
int  calc_phase(int real, int imag);

/**
 * @brief      This function servers to calculate real and imag parts.
 * @param[in]  Table1  -  A struct to store value or stand for numbers.
 * @param[in]  Table2  -  A struct to store value or stand for numbers..
 * @param[in]  fst_num -  The value stand for a start number.
 * @param[in]  scd_num -  The value stand for the next number.
 * @return     none.
 */
void dot_product(IQ_cb Table1,IQ_cb Table2,unsigned char fst_num,unsigned char scd_num);	//,IQ_Value rst_tb

/**
 * @brief      This function servers to get phase.
 * @param[in]  x_ptr  -  Pointer variable of the true phase value.
 * @param[in]  osr    -  The value use for calculation.
 * @return     The phase.
 */
//void dot_product(unsigned char first_num,unsigned char second_num);
int  phase_combine(int* x_ptr,int osr);

/**
 * @brief      This function servers to calculate the angel.
 * @param[in]  x -  The value need to be assignment.
 * @return     The angel.
 */
int  angle_trans(int x);	//input is 1024 times

/**
 * @brief      This function servers sure the angel value is accurate.
 * @param[in]  ptr1 -  Pointer variable of the angel.
 * @param[in]  ptr2 -  Pointer variable of the angel.
 * @return     0.
 */
char unwrap(int* ptr1,int* ptr2);

/**
 * @brief      This function servers to get the angel which already minus the average value.
 * @param[in]  ptr1 -  Pointer variable of the angel.
 * @return     none.
 */
void delate_average(int* ptr);

/**
 * @brief      This function servers to get the input angle.
 * @param[in]  ptr_packet  -  Pointer variable of the Rx packages.
 * @return     The angle.
 */
char get_input_angle(unsigned char *ptr_packet);

/**
 * @brief      This function servers to get a angle for Rx mode.
 * @param[in]  ptr_packet  -  Pointer variable of the Rx packages.
 * @return     The angle.
 */
unsigned int get_input_angle_for_polygon(unsigned char *ptr_packet);

/**
 * @brief      This function servers to store data to a specific place .
 * @param[in]  data_table  -  Pointer variable to store data.
 * @param[in]  ptr_packet  -  Pointer variable to support data.
 * @param[in]  number      -  The data numbers.
 * @return     none.
 */
void get_raw_data(unsigned char *data_table,unsigned char *ptr_packet,unsigned int number);

/**
 * @brief      This function servers copy a value to a destination place.
 * @param[in]  src  -  Pointer variable of the destination place.
 * @param[in]  dest -  Pointer variable of the source place.
 * @param[in]  len  -  The length of the value.
 * @return     none.
 */
void bcopy(register char * src, register char * dest, int len);

/**
 * @brief      This function servers copy a value to a destination place.
 * @param[in]  src -  Pointer variable of the destination place.
 * @param[in]  dest  -  Pointer variable of the source place.
 * @param[in]  len  -  The length of the value.
 * @return     none.
 */
void bbcopy(register char * src, register char * dest, int len);

/**
 * @brief      This function servers copy a value to a destination place.
 * @param[in]  out -  Pointer variable of the destination place.
 * @param[in]  in  -  Pointer variable of the source place.
 * @param[in]  length  -  The length of the value.
 * @return     none.
 */
void * memcpy(void * out, const void * in, unsigned int length);

/**
 * @brief      This function servers assignment a value to a destination place.
 * @param[in]  dest -  Pointer variable of the destination place.
 * @param[in]  val  -  Value need to assignment.
 * @param[in]  len  -  The length of the value.
 * @return     none.
 */
void * memset(void * dest, int val, unsigned int len);

/**
 * @brief      This function servers initialization a look up table.
 * @param[in]  none.
 * @return     none.
 */
void init_lookup_table(void);

/**
 * @brief      This function servers to shift the circuit.
 * @param[in]  func -  Structure stand for the parameter.
 * @return     Flag. 1 - shift failure 0 - shift success.
 */
unsigned char  circshift(Func_CB func);

/**
 * @brief      This function servers to transfer value to initialization.
 * @param[in]  src_tb -  Pointer value of the source table.
 * @param[in]  rst_tb -  Pointer value of the rst table.
 * @param[in]  number -  The number of the data.
 * @return     none.
 */
void trans_uchar_to_int(unsigned char* src_tb,int* rst_tb,unsigned int number);

/**
 * @brief      This function servers to initialization some GPIO for indicate.
 * @param[in]  none.
 * @return     none.
 */
void init_test_gpio(void);

/**
 * @brief      This function servers to initialization angel of 1,3,5,7.
 * @param[in]  none.
 * @return     none.
 */
void triangle_rf1357(void);

/**
 * @brief      This function servers to initialization angel of 2,4,6,8.
 * @param[in]  none.
 * @return     none.
 */
void triangle_rf2468(void);

/**
 * @brief      This function servers to initialization all the triangle.
 * @param[in]  none.
 * @return     none.
 */
void triangle_all_open(void);









