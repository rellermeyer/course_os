#include <chipset.h>
#include <stdio.h>
#include <string.h>

static char lower_case_digits[16] = "0123456789abcdef";
static char upper_case_digits[16] = "0123456789ABCDEF";

// base is between 2 and 16, inclusive
int print_int(char * buf,
              int buflen,
              int val,
              int base,
              int is_unsigned,
              int padding,
              char pad_char,
              int is_uppercase) {
    int max_len = buflen;
    int orig_max_len = max_len;
    int negate = 0;
    if (val < 0 && !is_unsigned) {
        val = -val;
        negate = 1;
    }
    unsigned int temp = val;

    if (max_len == 0) return orig_max_len - max_len;
    if (negate) {
        *buf = '-';
        buf++;
        max_len--;
        if (max_len == 0) return orig_max_len - max_len;
    }

    char tmp_buf[64];
    int ndigits = 0;
    while (temp != 0) {
        if (is_uppercase) {
            tmp_buf[ndigits] = upper_case_digits[temp % base];
        } else {
            tmp_buf[ndigits] = lower_case_digits[temp % base];
        }
        temp = temp / base;
        ndigits++;
    }

    // Zero-pad the output
    int i;
    if (padding > 0) {
        for (i = 0; i < padding - ndigits - negate; i++) {
            *buf = pad_char;
            buf++;
            max_len--;
            if (max_len == 0) return orig_max_len;
        }
    }

    // Output the digits
    for (i = ndigits - 1; i >= 0; i--) {
        *buf = tmp_buf[i];
        buf++;
        max_len--;
        if (max_len == 0) return orig_max_len;
    }
    if (ndigits == 0 && padding <= 0) {
        *buf = '0';
        buf++;
        max_len--;
        if (max_len == 0) return orig_max_len;
    }

    return orig_max_len - max_len;
}

// args must already have been started
// TODO: properly stop at buflen. Otherwise there is a buffer overflow.
int os_vsnprintf(char * buf, int buflen, const char * str_buf, va_list args) {
    if (buflen == 0) return 0;
    buflen--;
    if (buflen == 0) {
        buf[0] = 0;
        return 1;
    }
    int nwritten = 0;
    int t_arg;
    char * str_arg;
    int padding = -1;
    char pad_char = 0;
    while (*str_buf != '\0') {
        int n = 0;
        if (*str_buf == '%') {
            str_buf++;
        // This label is where we go after we've read an option.
        reread_switch:;
            switch (*str_buf) {
                case '0':
                    // Zero-padding... Read all the numbers.
                    // Then restart the switch statement.
                    padding = 0;
                    pad_char = '0';
                    while (*str_buf <= '9' && *str_buf >= '0') {
                        padding *= 10;
                        padding += *str_buf - '0';
                        str_buf++;
                    }
                    goto reread_switch;
                    break;
                case 'p':
                    *buf++ = '0';
                    *buf++ = 'x';
                case 'X':
                    t_arg = va_arg(args, int);
                    n = print_int(buf, buflen, t_arg, 16, 1, padding, pad_char, 1);
                    break;
                case 'x':
                    t_arg = va_arg(args, int);
                    n = print_int(buf, buflen, t_arg, 16, 1, padding, pad_char, 0);
                    break;
                case 'd':
                case 'i':
                    t_arg = va_arg(args, int);
                    n = print_int(buf, buflen, t_arg, 10, 0, padding, pad_char, 0);
                    break;
                case 'u':
                    t_arg = va_arg(args, int);
                    n = print_int(buf, buflen, t_arg, 10, 1, padding, pad_char, 0);
                    break;
                case 'c':
                    t_arg = va_arg(args, int);
                    *buf = t_arg;
                    n = 1;
                    break;
                case 's':
                    str_arg = va_arg(args, char *);
                    strncpy(buf, str_arg, buflen);
                    n = strlen(str_arg);
                    if (n > buflen) { n = buflen; }
                    break;
                case '%':
                    *buf = '%';
                    n = 1;
                    break;
            }
            // Reset all the options
            padding = -1;
        } else {
            *buf = *str_buf;
            n = 1;
        }
        buf += n;
        buflen -= n;
        nwritten += n;
        if (buflen <= 0) {
            // Return!
            break;
        }
        str_buf++;
    }
    buf[0] = 0;
    nwritten++;

    return nwritten;
}

int os_snprintf(char * buf, int buflen, const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = os_vsnprintf(buf, buflen, fmt, args);
    va_end(args);
    return n;
}

int puts(const char * s) {
    while (*s != '\0') {
        chipset.uart->putc(chipset.uart, *s, 0);
        s++;
    }

    return 0;
}

int kprintf(const char * str_buf, ...) {
    va_list args;
    va_start(args, str_buf);
    char buf[256];
    int n = os_vsnprintf(buf, 255, str_buf, args);
    va_end(args);
    puts(buf);
    return n;
}
