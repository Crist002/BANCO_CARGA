#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

void setup () {
rtc.adjust(DateTime(2021,5,3,12,10,00));
}

void loop () {
}
