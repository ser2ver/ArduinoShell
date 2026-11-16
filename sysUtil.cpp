#include <Arduino.h>
#include "sysUtil.h"

long pow10(int deg) {
  if (deg == 0) {
    return 1;
  }
  long x = 10;
  while (deg > 1) {
    x *= 10;
    --deg;
  }
  return x;
}

size_t intToStr(long num, char *buf, size_t pos, int digs) {
  if (num == 0) {
    while (digs > 0) {
      buf[pos++] = '0';
      --digs;
    }
    buf[pos] = '\0';
    return pos;
  }

  if (num < 0) {
    buf[pos++] = '-';
    num *= -1;
    --digs;
  }

  long d = 1000000;
  while (num / d == 0) {
    if (d - pow10(digs) < 0) {
      buf[pos++] = '0';
      --digs;
    }
    d /= 10;
  }

  while (d > 0) {
    buf[pos++] = '0' + num / d;
    num %= d;
    d /= 10;
  }
  buf[pos] = '\0';

  return pos;
}

long strToInt(const char *str, size_t &pos) {
  size_t sLen = strlen(str);

  for ( ; pos < sLen; ++pos) {
    if (!isspace(str[pos])) {
      break;
    }
  }

  long x = 0;
  for ( ; pos < sLen; ++pos) {
    if ((str[pos] < '0') || (str[pos] > '9')) {
      break;
    }
    x = x * 10 + (str[pos] - '0');
  }

  return x;
}

size_t hexToStr(uint32_t num, char *buf, size_t pos, int digs) {
  if (num == 0) {
    while (digs > 0) {
      buf[pos++] = '0';
      --digs;
    }
    buf[pos] = '\0';
    return pos;
  }

  int32_t d = 1 << 28;
  while (num / d == 0) {
    if (d - (1 << (4 * digs)) < 0) {
      buf[pos++] = '0';
      --digs;
    }
    d >>= 4;
  }

  while (d > 0) {
    if (num / d < 10) {
      buf[pos++] = '0' + num / d;
    } else {
      buf[pos++] = 'a' + num / d - 10;
    }
    num %= d;
    d >>= 4;
  }
  buf[pos] = '\0';

  return pos;
}

uint32_t strToHex(const char *str, size_t &pos) {
  size_t sLen = strlen(str);

  for ( ; pos < sLen; ++pos) {
    if (!isspace(str[pos])) {
      break;
    }
  }

  uint32_t x = 0;
  uint8_t ch;
  for ( ; pos < sLen; ++pos) {
    ch = str[pos];
    if (isalpha(ch)) {
      ch = tolower(ch);
    }
    if ((ch >= '0') && (ch <= '9')) {
      x = x * 16 + (ch - '0');
    } else if ((ch >= 'a') && (ch <= 'f')) {
      x = x * 16 + 10 + (ch - 'a');
    } else {
      break;
    }
  }

  return x;
}
