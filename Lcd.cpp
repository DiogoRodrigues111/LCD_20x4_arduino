/*
Copyright � 2006-2008 Hans-Christoph Steiner. All rights reserved. Copyright (c) 2010 Arduino LLC. All right reserved.
This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

----------------------------------------------------------

Implementations of the LCD Display for 20x4 segments.
Copyright (C) 2022  Diogo R. Roessler.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include "Lcd.h"

void Lcd::setRowOffsets(int row0, int row1, int row2, int row3, int row4)
{
	_row_offsets[0] = row0;
	_row_offsets[1] = row1;
	_row_offsets[2] = row2;
	_row_offsets[3] = row3;
	_row_offsets[4] = row4;
}

void Lcd::pulseEnable(void)
{
	digitalWrite(_enable_pin, LOW);
	delayMicroseconds(1);
	digitalWrite(_enable_pin, HIGH);
	delayMicroseconds(1); // enable pulse must be >450ns
	digitalWrite(_enable_pin, LOW);
	delayMicroseconds(100); // commands need > 37us to settle
}

void Lcd::send(uint8_t value, uint8_t mode)
{
	digitalWrite(_rs_pin, mode);

	// if there is a RW pin indicated, set it low to Write
	if (_rw_pin != 255)
	{
		digitalWrite(_rw_pin, LOW);
	}

	if (_displayfunction & LCD_8BITMODE)
	{
		write8bits(value);
	}
	else
	{
		write4bits(value >> 4);
		write4bits(value);
	}
}

__inline void Lcd::command(uint8_t value)
{
	send(value, LOW);
}

void Lcd::write4bits(uint8_t value)
{
	for (int i = 0; i < 4; i++)
	{
		digitalWrite(_data_pins[i], (value >> i) & 0x01);
	}

	pulseEnable();
}

void Lcd::setCursor(uint8_t col, uint8_t row)
{
	const size_t max_lines = ( sizeof(_row_offsets) / sizeof(*_row_offsets) );

	if (row >= max_lines)
	{
		row = max_lines - 1; // we count rows starting w/ 0
	}
	if (row >= _numlines)
	{
		row = _numlines - 1; // we count rows starting w/ 0
	}

	command(LCD_SETDDRAMADDR + (col + _row_offsets[row]));
}

void Lcd::write8bits(uint8_t value)
{
	for (int i = 0; i < 8; i++)
	{
		digitalWrite(_data_pins[i], (value >> i) & 0x01);
	}

	pulseEnable();
}

void Lcd::display()
{
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void Lcd::clear()
{
	command(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
	delayMicroseconds(2000);   // this command takes a long time!
}

void Lcd::begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
	uint8_t line[5] { lines };

	line[0] = 0x00;
	line[1] = LCD_1LINE;
	line[2] = LCD_2LINE;
	line[3] = LCD_3LINE;
	line[4] = LCD_4LINE;

	switch (line[lines])
	{
	case 0:
		_displayfunction = LCD_1LINE + 0x00;
		break;

	case 1:
		_displayfunction = (LCD_2LINE << 2);
		break;

	case 2:
		_displayfunction = (LCD_3LINE << 3);
		break;

	case 3:
		_displayfunction = (LCD_4LINE << 4);
		break;

	default:
		if (line[lines] > 1)
			_numlines = line[lines];
	}

	setRowOffsets(0x00, LCD_1LINE, LCD_2LINE, LCD_3LINE +4, LCD_4LINE +4); /* 0x40 */

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != LCD_5x8DOTS) && (lines > 1))
	{
		_displayfunction |= LCD_5x10DOTS;
	}

	pinMode(_rs_pin, OUTPUT);

	// we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
	if (_rw_pin != 255)
	{
		pinMode(_rw_pin, OUTPUT);
	}

	pinMode(_enable_pin, OUTPUT);

	// Do these once, instead of every time a character is drawn for speed reasons.
	for (int i = 0; i < ((_displayfunction & LCD_8BITMODE) ? 8 : 4); ++i)
	{
		pinMode(_data_pins[i], OUTPUT);
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
	delayMicroseconds(50000);
	// Now we pull both RS and R/W low to begin commands
	digitalWrite(_rs_pin, LOW);
	digitalWrite(_enable_pin, LOW);

	if (_rw_pin != 255)
	{
		digitalWrite(_rw_pin, LOW);
	}

	// put the LCD into 4 bit or 8 bit mode
	if (!(_displayfunction & LCD_8BITMODE))
	{
		// this is according to the hitachi HD44780 datasheet
		// figure 24, pg 46

		// we start in 8bit mode, try to set 4 bit mode
		write4bits(0x03);
		delayMicroseconds(4500); // wait min 4.1ms

		// second try
		write4bits(0x03);
		delayMicroseconds(4500); // wait min 4.1ms

		// third go!
		write4bits(0x03);
		delayMicroseconds(150);

		// finally, set to 4-bit interface
		write4bits(0x02);
	}
	else
	{
		// this is according to the hitachi HD44780 datasheet
		// page 45 figure 23

		// Send function set command sequence
		command(LCD_FUNCTIONSET | _displayfunction);
		delayMicroseconds(4500); // wait more than 4.1ms

		// second try
		command(LCD_FUNCTIONSET | _displayfunction);
		delayMicroseconds(150);

		// third go
		command(LCD_FUNCTIONSET | _displayfunction);
	}

	// finally, set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();

	// clear it off
	clear();

	// Initialize to default text direction (for romance languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
}

void Lcd::init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
			   uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			   uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
	_rs_pin = rs;
	_rw_pin = rw;
	_enable_pin = enable;

	_data_pins[0] = d0;
	_data_pins[1] = d1;
	_data_pins[2] = d2;
	_data_pins[3] = d3;
	_data_pins[4] = d4;
	_data_pins[5] = d5;
	_data_pins[6] = d6;
	_data_pins[7] = d7;

	if (fourbitmode) {
		
		//if (fourbitmode) {
		//	_displayfunction = LCD_4BITMODE | LCD_4LINE | LCD_5x8DOTS;
		//	begin(16, 4);
		//}
		
		_displayfunction = LCD_4BITMODE | LCD_4LINE | LCD_5x8DOTS;
	}
	else {

		//if (fourbitmode) {
		//	_displayfunction = LCD_4BITMODE | LCD_3LINE | LCD_5x8DOTS;
		//	begin(16, 4);
		//}

		_displayfunction = LCD_8BITMODE | LCD_4LINE | LCD_5x8DOTS;
	}

	begin(16, 1);
}

__inline size_t Lcd::write(uint8_t value)
{
	send(value, HIGH);
	return 1;
}

Lcd::Lcd(uint8_t rs, uint8_t rw, uint8_t enable,
		 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
	init(0, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

Lcd::Lcd(uint8_t rs, uint8_t enable,
		 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
	init(0, rs, 255, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

Lcd::Lcd(uint8_t rs, uint8_t rw, uint8_t enable,
		 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
	init(1, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

Lcd::Lcd(uint8_t rs, uint8_t enable,
		 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
	init(1 , rs, 255, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

Lcd::~Lcd() {}
