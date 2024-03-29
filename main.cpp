#include <Arduino.h>
#include "Lcd.h"

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
Lcd lcd(rs, en, d4, d5, d6, d7);

void setup()
{
	lcd.begin(20, 4);

	lcd.setCursor(0, 4);
	lcd.print("Hello, World!");
}

void loop()
{
	lcd.setCursor(0, 1);
	lcd.print(millis() /1000);
}
