#include "my_misc.h"
#include "stdint.h"

int inRange(int cur, int tag , int range)
{
  if((cur<=tag+range)&&(cur>=tag-range))
    return 1;
  return 0;
}

int limitValue(int min, int value, int max)
{
  if(value<min)
    return min;
  if(value>max)
    return max;
  return value;
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int intToString(char* str, int n, int radix, char isNegative)  //Express integers as characters  //将整数表达成字符形态
{
  int i = 0, j = 0, remain = 0;
  int len = 0;
  char tmp = 0;

  if (n < 0)
  {
    isNegative = 1;
    n = -n;
  }

  do
  {
    remain = n % radix;
    if (remain > 9)
      str[i] = remain - 10 + 'A';  //For hex, 10 will be represented as A  //为了十六进制，10将表示成A
    else
      str[i] = remain + '0';      //Integer + '0' = ASCII code corresponding to the integer  //将整数+'0' = 整数对应的ASCII码
    i++;
  } while (n /= radix);

  if (isNegative == 1)
    str[i++] = '-';
  str[i] = '\0';
  len = i;

  for (i--, j = 0; j <= i; j++, i--)  //25%10 = 5,25/10 = 2,2%10 = 2,2/10 = 0，So the result in str is upside down.  //所以str中结果是倒置的，翻转一下
  {
    tmp = str[j];
    str[j] = str[i];
    str[i] = tmp;
  }

  return len;
}

const uint32_t POW_10[] = {
1, 10,100, 1000, 10000, 100000, 1000000, 10000000,
100000000, 1000000000
};

int my_vsprintf(char *buf, const char *fmt, my_va_list args)
{
  char* p;
  my_va_list p_next_arg = args;
  uint8_t bit_width[2] = {0, 6};
  uint8_t bit_sel = 0;

  for (p=buf; *fmt; fmt++)
  {
    if (*fmt != '%')
    {
      *p++ = *fmt;
      continue;
    }
    bit_width[0] = 0;
    bit_width[1] = 6;
    bit_sel = 0;

    repeat:
    fmt++;
    if (*fmt >= '0' && *fmt <= '9' && bit_sel < 2)
    {
      bit_width[bit_sel] = *fmt - '0';
      goto repeat;
    }
    switch (*fmt)
    {
      case 'd': //Decimal integer  //十进制整数
      {
        int n = my_va_arg(p_next_arg, int);
        p += intToString(p, n, 10, 0);
        break;
      }
      case 'x': //Hexadecimal integer  //十六进制整数
      {
        int n = my_va_arg(p_next_arg, int);
        p += intToString(p, n, 16, 0);
        break;
      }
      case 'f': //Floating point number  //浮点数
      {
        if((unsigned long)p_next_arg & 0x7)   //Variable parametersFloating point numbers are double by default.Ensure 8-byte memory alignment  //可变参 浮点数默认是double类型 保证内存8字节对齐
        {
          p_next_arg = (my_va_list)((unsigned long)p_next_arg + 0x7);
          p_next_arg = (my_va_list)((unsigned long)p_next_arg & 0xFFFFFFF8);
        }
        double f = my_va_arg(p_next_arg, double);  //%f, output floating point number  //%f，输出浮点数
        int n = (int)f;
        p += intToString(p, n, 10, f < 0);
        *p++ = '.';

        double d = ABS(f - n) + 0.5/MIN(1000000, POW_10[bit_width[1]]);
        for(int i=0; i < MIN(6, bit_width[1]); i++)
        {
          d *= 10;
          *p++ = (((int)d) % 10) + '0';
        }
        break;
      }
      case 'c': //Single ASCII character  //单个 ASCII 字符
      {
        *p++ = my_va_arg(p_next_arg, int);
        break;
      }
      case 's': //String  //字符串
      {
        char *str = my_va_arg(p_next_arg, char *);
        for (; *str != 0; )
        {
          *p++ = *str++;
        }
        break;
      }
      case '%': //
      {
        *p++ = '%';
        break;
      }
      case '.':
      {
        bit_sel++;
        goto repeat;
      }
      default:
      {
        break;
      }
    }
  }
  *p++ = 0;
  return (p - buf);
}

void my_sprintf(char *buf, const char *fmt, ...)
{
  my_va_list ap;
  my_va_start(ap, fmt);
  my_vsprintf(buf, fmt, ap);
  my_va_end(ap);
}
