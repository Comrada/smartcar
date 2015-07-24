/**
 * @author  Pavel Lobyrin
 * @email   pavel@lobyrin.ru
 * @website http://lobyrin.de
 * @link    https://github.com/Comrada/SmartCar
 * @version v1.0
 * @ide     CooCox
 * @license GNU GPL v3
 * @brief   Main module of SmartCar
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

#ifndef MAIN_H
#define MAIN_H

#include "stm32f4xx.h"

#define USART_BAUDRATE 115200

static int RCMode = 1;
static const int USARTLog = 1;
static float Vbat = 0.00f;
static float CoreTemp = 0.00f;
static uint16_t DevSign = 0x0000;
static uint16_t DevRev = 0x0000;
static uint32_t DevID[3] = {0x0, 0x0, 0x0};
static int FlashSize = 0;

#endif
