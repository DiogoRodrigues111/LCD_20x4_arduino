/*
Copyright © 2006-2008 Hans-Christoph Steiner. All rights reserved. Copyright (c) 2010 Arduino LLC. All right reserved.
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

#ifndef LCD_H_
#define LCD_H_

#include <Print.h>
#include <Arduino.h>

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_4LINE 0x50
#define LCD_3LINE 0x10
#define LCD_2LINE 0x40
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class Lcd : public Print
{

public:

Lcd::Lcd(uint8_t rs, uint8_t rw, uint8_t enable,
				     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
				     uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

Lcd::Lcd(uint8_t rs, uint8_t enable,
				     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
				     uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

Lcd::Lcd(uint8_t rs, uint8_t rw, uint8_t enable,
				     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

Lcd::Lcd(uint8_t rs,  uint8_t enable,
				     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

void init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
	    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
	    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    
	virtual Lcd::~Lcd();

  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

  void clear();
  void home();

void send(uint8_t, uint8_t);
  void write4bits(uint8_t);
  void write8bits(uint8_t);
  void pulseEnable();

  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void autoscroll();
  void noAutoscroll();

  void setRowOffsets(int row0, int row1, int row2, int row3, int row4);
  void createChar(uint8_t, uint8_t[]);
  void setCursor(uint8_t, uint8_t); 
  virtual size_t write(uint8_t);
  void command(uint8_t);

using Print::write;

public:
	uint8_t _data_pins[8];
	uint8_t _rs_pin;
	uint8_t _rw_pin;
	uint8_t _enable_pin;
	uint8_t _displayfunction;
	uint8_t fourbitmode;
	uint8_t _numlines;
	uint8_t _row_offsets[5];
	uint8_t _displaymode;
	uint8_t _displaycontrol;
};

#endif /* LCD_H_ */
