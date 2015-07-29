/**
 * |----------------------------------------------------------------------
 * | Copyright (C) Pavel Lobyrin, 2015
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */

#include "json.h"
#include "main.h"

void rc_printMotorState(Motor motor)
{
	if (RCMode != 1) return;
	char * json;
	json = malloc(512);

	sprintf(json, "{ \"motor\" : { \"Num\" : %d, \"mode\" : \"%s\", \"direction\" : \"%s\", \"speed\" : \"%d\" } }\r\n", motor.Num, MotorModeToStr(motor.Mode), MotorDirectionToStr(motor.Direction), motor.Speed);
	sendToUSART(json);
	free(json);
}

char * MotorModeToStr(uint8_t mode)
{
	char * str = malloc(20);
	switch (mode) {
		case Mode_Stop:
			str = "Mode_Stop";
			break;
		case Mode_Active:
			str = "Mode_Active";
			break;
		default:
			str = "Unknown";
			break;
	}
	return str;
}

char * MotorDirectionToStr(uint8_t direction)
{
	char * str = malloc(20);
	switch (direction) {
		case Direction_None:
			str = "Direction_None";
			break;
		case Direction_Forward:
			str = "Mode_Active";
			break;
		case Direction_Backward:
			str = "Direction_Backward";
			break;
		default:
			str = "Unknown";
			break;
	}
	return str;
}
