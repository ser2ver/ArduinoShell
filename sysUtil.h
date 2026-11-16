#ifndef _SYS_UTIL_H_
#define _SYS_UTIL_H_

#include <Arduino.h>

long pow10(int deg);

size_t intToStr(long num, char *buf, size_t pos, int digs = 1);
long strToInt(const char *str, size_t &pos);

size_t hexToStr(uint32_t num, char *buf, size_t pos, int digs = 1);
uint32_t strToHex(const char *str, size_t &pos);

#endif // _SYS_UTIL_H_
