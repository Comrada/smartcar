/**
 * @author  Pavel Lobyrin
 * @email   pavel@lobyrin.ru
 * @website http://lobyrin.de
 * @link    https://github.com/Comrada/SmartCar
 * @version v1.0
 * @ide     CooCox
 * @license GNU GPL v3
 * @brief   Low level initialization for SmartCar
 *
@verbatim
   ----------------------------------------------------------------------
    Copyright (C) Pavel Lobyrin, 2015

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
@endverbatim
 */

#ifndef _MCU_INIT_H
#define _MCU_INIT_H

#include "stm32f4xx_adc.h"
#include "stm32f4xx_rcc.h"

/**
 * @brief Unique ID register address location
 */
#define ID_UNIQUE_ADDRESS		0x1FFF7A10

/**
 * @brief Flash size register address
 */
#define ID_FLASH_ADDRESS		0x1FFF7A22

/**
 * @brief Device ID register address
 */
#define ID_DBGMCU_IDCODE		0xE0042000

/**
 * @brief  Get STM32F4xx device signature
 * @retval Device signature, bits 11:0 are valid, 15:12 are always 0.
 *           - 0x0413: STM32F405xx/07xx and STM32F415xx/17xx)
 *           - 0x0419: STM32F42xxx and STM32F43xxx
 *           - 0x0423: STM32F401xB/C
 *           - 0x0433: STM32F401xD/E
 *           - 0x0431: STM32F411xC/E
 */
#define MCU_GetSignature()	((*(uint16_t *) (ID_DBGMCU_IDCODE)) & 0x0FFF)

/**
 * @brief  Get STM32F4xx device revision
 * @note   Defined as macro to get maximal response time
 * @retval Device revision value
 *           - 0x1000: Revision A
 *           - 0x1001: Revision Z
 *           - 0x1003: Revision Y
 *           - 0x1007: Revision 1
 *           - 0x2001: Revision 3
 */
#define MCU_GetRevision()		(*(uint16_t *) (ID_DBGMCU_IDCODE + 2))

/**
 * @brief  Get STM32F4xx device's flash size in kilo bytes
 * @note   Defined as macro to get maximal response time
 * @retval Flash size in kilo bytes
 */
#define MCU_GetFlashSize()	(*(uint16_t *) (ID_FLASH_ADDRESS))

/**
 * @brief  Get unique ID number in 32-bit format
 * @note   STM32F4xx has 96bits long unique ID, so 3 4-bytes values are available for read in 32-bit format
 * @param  x: Byte number: specify which part of 16 bits you want to read
 *               - Values between 0 and 2 are allowed
 * @retval Unique ID address
 */
#define MCU_GetUnique32(x)	((x >= 0 && x < 3) ? (*(uint32_t *) (ID_UNIQUE_ADDRESS + 4 * (x))) : 0)

void initMCU(void);
void initADC(void);
void getVbat(void);
void getTemp(void);

#endif
