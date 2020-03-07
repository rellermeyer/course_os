/********************************************************************
 * klibc.c
 *
 * (Any collaborators, please add your name)
 * Author: Jared McArthur, Taylor Smith, Sheldon Sandbekkhaug, Kaelen Haag, Collin Massey
 *
 * Last edited: 4 December 2015
 *
 * Purpose:  Provide basic libc funtionality for CourseOS
 *     This file provides function implementations
 *     for skels in libc.h
 *
 * Usage:  Compile into kernel. Adaptations of normal libc functions
 *   can be used by prepending os_ suffix.
 *
 * Notes:  The following were adapted directly from musl-libc:
 *               memcmp, memset, strcmp, strchrnul, strcpy, strlen, strtok
 ********************************************************************/
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include <stdio.h>
#include <mem_alloc.h>

//FIXME: decouple
#include <interrupt.h>

#define LOWER_CASE 0
#define UPPER_CASE 1
#define NO_CASE 2

// Helpers for MUSL String functions
#define UCHAR_MAX 0xFF
#define ALIGN (sizeof(os_size_t))
#define ONES ((os_size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) (((x)-ONES) & (~(x)) & HIGHS)

/*4-17-15: - Prakash
 * panic() added
 - Currrently states the panic and stalls the machine
 */
void panic() {
    disable_interrupt(BOTH);
    kprintf("Kernel panic!\n");
    kprintf("\n     )                        (                     \n");
    kprintf("  ( /(                   (    )\\ )                  \n");
    kprintf("  )\\()) (  (           ( )\\  (()/(   )      (       \n");
    kprintf("|((_)\\ ))\\ )(   (     ))((_)  /(_)| /(  (   )\\  (   \n");
    kprintf("|_ ((_)((_|()\\  )\\ ) /((_)   (_)) )(_)) )\\ |(_) )\\  \n");
    kprintf("| |/ (_))  ((_)_(_/((_))| |  | _ ((_)_ _(_/((_)((_) \n");
    kprintf("  ' </ -_)| '_| ' \\)) -_) |  |  _/ _` | ' \\)) / _|  \n");
    kprintf(" _|\\_\\___||_| |_||_|\\___|_|  |_| \\__,_|_||_||_\\__|  ");
    SemihostingCall(OSSpecific);
    SLEEP;
    __builtin_unreachable();
}

void splash() {

//    kprintf("\n\n");
//    kprintf("\t ██████╗ ██████╗ ██╗   ██╗██████╗ ███████╗███████╗ ██████╗ ███████╗\n");
//    kprintf("\t██╔════╝██╔═══██╗██║   ██║██╔══██╗██╔════╝██╔════╝██╔═══██╗██╔════╝\n");
//    kprintf("\t██║     ██║   ██║██║   ██║██████╔╝███████╗█████╗  ██║   ██║███████╗\n");
//    kprintf("\t██║     ██║   ██║██║   ██║██╔══██╗╚════██║██╔══╝  ██║   ██║╚════██║\n");
//    kprintf("\t╚██████╗╚██████╔╝╚██████╔╝██║  ██║███████║███████╗╚██████╔╝███████║\n\n");
//
    kprintf("\n\n");
    kprintf("\t ██████╗██╗   ██╗██████╗ ███████╗███████╗██████╗    ██████╗ ███████╗\n");
    kprintf("\t██╔════╝██║   ██║██╔══██╗██╔════╝██╔════╝██╔═══██╗ ██╔═══██╗██╔════╝\n");
    kprintf("\t██║     ██║   ██║██████╔╝███████╗█████╗  ██║   ██║ ██║   ██║███████╗\n");
    kprintf("\t██║     ██║   ██║██╔══██╗╚════██║██╔══╝  ██║   ██║ ██║   ██║╚════██║\n");
    kprintf("\t╚██████╗╚██████╔╝██║  ██║███████║███████╗██████╔╝  ╚██████╔╝███████║\n\n");
}

/*4-17-15: - Prakash
 _assert_fail() added
 - This is a helper function for the assert() macro
 */
int _assert_fail(char *_file, unsigned int _line, char *_func) {
    kprintf("ASSERT FAILURE: %s:%u: %s\n", _file, _line, _func);
    panic();
    return 1;
}



/* Returns a pointer to the first instance of c in s, like indexOf().
 If c is not found, then return a pointer to the NULL character at
 the end of String s.
 */
char *__strchrnul(char *s, char c) {
    os_size_t *w, k;

    if (!c)
        return (char *) s + strlen(s);

    for (; (uintptr_t) s % ALIGN; s++)
        if (!*s || *(unsigned char *) s == c)
            return (char *) s;
    k = ONES * c;
    for (w = (void *) s; !HASZERO(*w) && !HASZERO(*w ^ k); w++);
    for (s = (void *) w; *s && *(unsigned char *) s != c; s++);
    return (char *) s;
}

/* A re-entrant function that returns a substring of s. The substring starts
 at the beginning of the string or the previous index of one of the
 delimiter characters (indicated by sep). The substring ends at the next
 delimeter character (indicated by sep).
 */
char *os_strtok(char *s, char *sep) {
    static char *p;
    if (!s && !(s = p))
        return NULL;
    s += os_strspn(s, sep);
    if (!*s)
        return p = 0;
    p = s + os_strcspn(s, sep);
    if (*p)
        *p++ = 0;
    else
        p = 0;
    return s;
}

/* Returns the length of the initial segment of s that only includes
 the characters in c.
 */
os_size_t os_strspn(char *s, char *accept) {
    char c = s[0]; // The character in s being checked
    int length = 0;

    // Check each character in s
    while (c != 0) {
        bool ok = false;

        // Check against each character in accept
        int i;
        for (i = 0; i < strlen(accept); i++) {
            if (c == accept[i]) {
                ok = true;
            }
        }

        if (ok == true) {
            // If c matched any character in accept, continue
            length++;
            c = s[length];
        } else {
            // If did not match any character in accept, we are done
            return length;
        }
    }
    return length;
}

/* Returns the length of the initial segment of s that does not contain
 any characters in string c.
 */
os_size_t os_strcspn(char *s, char *reject) {
    char c = s[0]; // The character in s being checked
    int length = 0;

    // Check each character in s
    while (c != 0) {
        // Check against each character in reject
        int i;
        for (i = 0; i < strlen(reject); i++) {
            if (c == reject[i]) {
                return length;
            }
        }

        // If c did not match any reject characters, continue
        length++;
        c = s[length];
    }
    return length;
}

// Return string converted to int form, or 0 if not applicable
// Necessary comments provided in atof() (next function)
int katoi(char *string) {
    if (!string)
        return 0;

    int integer = 0;
    int sign = 1;

    if (*string == '-') {
        sign = -1;
        string++;
    } else if (*string == '+') {
        string++;
    }

    while (*string != '\0') {
        if (*string >= '0' && *string <= '9') {
            integer *= 10;
            integer += (*string - '0');
        } else {
            return 0;
        }
    }
    return sign * integer;
}

// Return string converted to double form, or 0 if not applicable
double katof(char *string) {
    if (!string)
        return 0.0;

    double integer = 0.0; // before decimal
    double fraction = 0.0; // after decimal
    int sign = 1; // positive or negative?
    int divisor = 1; // used to push fraction past decimal point
    bool after_decimal = false; // decimal point reached?

    // Check if string includes sign (including a "+")
    if (*string == '-') {
        sign = -1;
        string++; // progress to next char
    } else if (*string == '+') {
        string++;
    }

    while (*string != '\0') {
        if (*string >= '0' && *string <= '9') {
            if (after_decimal) {
                fraction *= 10; // progress to next position in integer
                fraction += (*string - '0'); // add integer form of current number in string
                divisor *= 10;
            } else {
                integer *= 10; // progress to next position in integer
                integer += (*string - '0'); // add integer form of current number in string
            }
        } else if (*string == '.') {
            if (after_decimal)
                return 0.0; // more than one '.'
            after_decimal = true;
        } else {
            return 0.0; // current char in string is not a number or '.'
        }
        string++;
    }
    return sign * (integer + (fraction / divisor));
}

// Return string converted to long int form, or 0 if not applicable
long int katol(char *string) {
    return (long int) katoi(string);
}

/*
// Same a
#include "../../old/include/klibc.h"s katof, but makes endptr point to the string which comes after the number
double kstrtod(const char *string, char **endptr)
{
	if (!string)
		return 0.0;

	double integer = 0.0;
	double fraction = 0.0;
	int sign = 1;
	int divisor = 1;
	Boolean after_decimal = FALSE;
	Boolean result_found = FALSE;
	double result = 0.0;

	if (*string == '-')
	{
		sign = -1;
		string++;
	}
	else if (*string == '+')
		string++;
}

while (!result_found)
{
	if (*string >= '0' && *string <= '9')
	{
		if (after_decimal)
		{
			fraction *= 10;
			fraction += (*string - '0');
			divisor *= 10;
		}
		else
		{
			integer *= 10;
			integer += (*string - '0');
		}
		string++;
	}
	else if (*string == '.')
	{
		if (after_decimal)
		return 0.0;
		after_decimal = true;
		string++;
	}
	else
	{
		result = sign * (integer + (fraction/divisor));
		result_found = true;
	}
}

*endptr = string;

return result;
}
*/

//Same as katol, but makes endptr point to the string which comes after the number
long int kstrtol(char *string, char **endptr) {
    if (!string)
        return 0;

    long int integer = 0;
    long int result = 0;
    bool result_found = false;
    int sign = 1;

    if (*string == '-') {
        sign = -1;
        string++;
    } else if (*string == '+') {
        string++;
    }

    while (!result_found) {
        if (*string >= '0' && *string <= '9') {
            integer *= 10;
            integer += (*string - '0');
            string++;
        } else {
            result = sign * integer;
            result_found = true;
        }
    }
    *endptr = (char *) string;

    return result;
}

// NOTE potentially expand these features. offer more
// memory stats
uint32_t km_size() {
    return mem_get_heap_size();
}



unsigned int rand() {
    static unsigned int z1 = 12345, z2 = 67891, z3 = 11121, z4 = 31415;
    unsigned int b;
    b = ((z1 << 6) ^ z1) >> 13;
    z1 = ((z1 & 4294967294U) << 18) ^ b;
    b = ((z2 << 2) ^ z2) >> 27;
    z2 = ((z2 & 4294967288U) << 2) ^ b;
    b = ((z3 << 13) ^ z3) >> 21;
    z3 = ((z3 & 4294967280U) << 7) ^ b;
    b = ((z4 << 3) ^ z4) >> 12;
    z4 = ((z4 & 4294967168U) << 13) ^ b;
    return (z1 ^ z2 ^ z3 ^ z4);
}
