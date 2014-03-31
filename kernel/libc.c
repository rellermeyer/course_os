/********************************************************************
*	libc.c
*
*	Author: Jared McArthur	// any collaborators, please add name
*
*	Date last edit: 28 March 2014
*
*	Purpose:	Provide basic libc funtionality for CourseOS
*			This file provides function implementations
*			for skels in libc.h
*
*	Usage:	Compile into kernel. Adaptations of normal libc functions
*		can be used by prepending os_ suffix.
*
*	Notes:	memcmp and strcmp adapted directly from musl-libc
********************************************************************/

#include "include/libc.h"
#include <stdarg.h>
#include <stdint.h>

#define LOWER_CASE 0
#define UPPER_CASE 1
#define NO_CASE 2

static char lower_case_digits[16] = "0123456789abcdef";
static char upper_case_digits[16] = "0123456789ABCDEF";

/* string.h type functionality for comparing strings or mem blocks */
int os_memcmp ( const void *left, const void *right, size_t num )
{
	const unsigned char *l = left, *r = right;
	for ( ; num && *l == *r; num--, l++, r++ );
	return num ? *l - *r : 0;
}

int os_strcmp ( const char *left, const char *right)
{
	for (; *left == *right && *left; left++, right++);
	return *(unsigned char *)left - *(unsigned char *)right;
}

/* TODO: create print function for kernel debugging purposes */

/*
void print_number_using_base(int num, int base, int letter_case, int use_sign){
  int temp = num;
  int count_digits = 0;
  char buf[100];
  if (temp == 0){
    print_uart0('0');
  } else if (use_sign && temp < 0) {
    print_uart0('-');
    temp = -temp;
  }
  while(temp != 0){
    if(letter_case == UPPER_CASE){
      buf[count_digits] = upper_case_digits[(unsigned long) temp % (unsigned) base];
    } else {
      buf[count_digits] = lower_case_digits[(unsigned long) temp % (signed) base];
    }
    temp = ((unsigned long) temp / (unsigned) base);
    count_digits++;
  }
  while(count_digits > 0){
    char tmp_str[2] = "";
    tmp_str[0] = buf[count_digits-1]; 
    tmp_str[1] = '\0';
    print_uart0(tmp_str);
    count_digits--;
  }
}



int os_printf(const char *str_buf, ...) {
  va_list args;
  int t_arg;
  va_start(args, str_buf);
  int count_args_found = 0;
  while(*str_buf != '\0') {
    if(*str_buf == '%') {
      str_buf++;
      switch (*str_buf) {
        case 'X':
          print_number_using_base(va_arg(args, int), 16, UPPER_CASE, 0);
          break;
        case 'x':
          t_arg = va_arg(args,int);
          print_number_using_base(t_arg, 16, LOWER_CASE, 0);
          break;
        case 'd':
          print_number_using_base(va_arg(args, int), 10, NO_CASE, 1);
          break;
        case 'c':
          t_arg = va_arg(args,int);
          char tmp_str[2] = "";
          tmp_str[0] = t_arg; 
          tmp_str[1] = '\0';
          print_uart0(tmp_str);
          break;
        case '%':
          print_uart0("%");
          break;
      }
    } else {
      char tmp_str[2] = "";
      tmp_str[0] = (uint32_t) *str_buf; 
      tmp_str[1] = '\0';
      print_uart0(tmp_str);
    }
    str_buf++;
  }
  va_end(args);
}
*/

int main()
{ return 0; }
