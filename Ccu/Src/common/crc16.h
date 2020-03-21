/**************************************************************************//**
 * @file crc.h
 * @brief CRC16 routines for XMODEM and verification.
 * @version x.xx
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef CRC16_H
#define CRC16_H

unsigned short CRC16_calc(unsigned short crc, const void *start, int len);

#endif
