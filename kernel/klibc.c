/********************************************************************
*	klibc.c
*
*	Author: Jared McArthur, Taylor Smith	// any collaborators, please add name
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

#include "include/klibc.h"
#include "include/global_defs.h"
#include "include/mem_alloc.h"
#include "uart.c"
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

void print_hex(int val, int CASE)
{
  int temp = val;
  int count_digits = 0;
  char buf[100];
  int CHAR_MASK = 0xF;
  if (temp == 0){
    //printf("0");
    print_char_uart0('0');
  } 
  while((temp != 0) && (count_digits < 8))
  {
    int index = temp & CHAR_MASK;
    if(CASE == UPPER_CASE){
      buf[count_digits] = upper_case_digits[index];
    } else {
      buf[count_digits] = lower_case_digits[index];
    }
    temp = temp >> 4;
    count_digits += 1;
  }
  while(count_digits > 0){
    //printf("%c", buf[count_digits-1]);
    print_char_uart0(buf[count_digits - 1]);
    count_digits--;
  }
}

print_dec(int val){
  int temp = val;
  int count_digits = 0;
  char buf[100];
  if (temp == 0){
    //printf("0");
    print_uart0("0");
  } else if (temp < 0) {
    //printf("-");
    print_uart0("-");
    temp = -temp;
  }
  while(temp != 0){
    int index = temp % 10;
    buf[count_digits] = upper_case_digits[index];
    temp = temp / 10;
    count_digits += 1;
  }
  while(count_digits > 0){
    //printf("%c", buf[count_digits-1]);
    print_char_uart0(buf[count_digits - 1]);
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
          t_arg = va_arg(args,int);
          print_hex(t_arg, UPPER_CASE);
          break;
        case 'x':
          t_arg = va_arg(args,int);
          print_hex(t_arg, LOWER_CASE);
          break;
        case 'd':
          t_arg = va_arg(args,int);
          print_dec(t_arg);
          break;
        case 'c':
          t_arg = va_arg(args,int);
          print_char_uart0(t_arg);
          //printf("%c", t_arg);
          break;
        case '%':
          print_uart0("%");
          //printf("%%");
          break;
      }
    } else {
      print_char_uart0(*str_buf);
      //printf("%c", *str_buf);
    }
    str_buf++;
  }
  va_end(args);
}

char* strtok(char *str, const char *delimeters) {
  char* token = str;
  //const char* temp_del;

  while(*str != NULL) {
    //temp_del = delimeters;

    //while(*str != *temp_del || *temp_del != NULL) {
    //  *token = *str;
    //  ++token;
    //  ++temp_del;
    //}
    printf("%c", *str);
    ++str;
  }
  printf("\n");
  return token;
}

/*char** string_slice(char *inputs, char delimitter)
{
  char **results = mem_alloc(sizeof(char*) * 16);
  int i = 0;
  int args = 0;
  while(inputs[i] != 0)
  {
    char *argument = mem_alloc(sizeof(char*) * 50);
    int j = 0;
    while(inputs[i] != delimitter)
    {
      argument[j] = inputs[i];
      i++;
      j++;
    }
    i++;//move past delimitter
    results[args] = argument;
    args++;
  }
  return results;

}*/

int main()
{ 
  /* char * test = "does this work";
  char delim = "\t";
  char** sliced = string_slice(test, delim);
  return 0; */
  char str[] ="- This, a sample string.";
  char * pch;
  printf ("Splitting string \"%s\" into tokens:\n",str);
  pch = strtok (str," ,.-");
  while (pch != NULL)
  {
    printf ("%s\n",pch);
    pch = strtok (NULL, " ,.-");
  }
  return 0;
}
