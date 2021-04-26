#ifndef ANSI_ESCAPE_H
#define ANSI_ESCAPE_H
#include <stdio.h>

// 8-bit foreground colours
#define ANSI_BLACK   "\e[30m"
#define ANSI_RED     "\e[31m"
#define ANSI_GREEN   "\e[32m"
#define ANSI_YELLOW  "\e[33m"
#define ANSI_BLUE    "\e[34m"
#define ANSI_MAGNETA "\e[35m"
#define ANSI_CYAN    "\e[36m"
#define ANSI_WHITE   "\e[37m"

// bright foreground colours
#define ANSI_BOLD_BLACK   "\e[30;1m"
#define ANSI_BOLD_RED     "\e[31;1m"
#define ANSI_BOLD_GREEN   "\e[32;1m"
#define ANSI_BOLD_YELLOW  "\e[33;1m"
#define ANSI_BOLD_BLUE    "\e[34;1m"
#define ANSI_BOLD_MAGNETA "\e[35;1m"
#define ANSI_BOLD_CYAN    "\e[36;1m"
#define ANSI_BOLD_WHITE   "\e[37;1m"

// 8-bit background colours
#define ANSI_BG_BLACK   "\e[40m"
#define ANSI_BG_RED     "\e[41m"
#define ANSI_BG_GREEN   "\e[42m"
#define ANSI_BG_YELLOW  "\e[43m"
#define ANSI_BG_BLUE    "\e[44m"
#define ANSI_BG_MAGNETA "\e[45m"
#define ANSI_BG_CYAN    "\e[46m"
#define ANSI_BG_WHITE   "\e[47m"

// bright foreground colours
#define ANSI_BG_BOLD_BLACK   "\e[40;1m"
#define ANSI_BG_BOLD_RED     "\e[41;1m"
#define ANSI_BG_BOLD_GREEN   "\e[42;1m"
#define ANSI_BG_BOLD_YELLOW  "\e[43;1m"
#define ANSI_BG_BOLD_BLUE    "\e[44;1m"
#define ANSI_BG_BOLD_MAGNETA "\e[45;1m"
#define ANSI_BG_BOLD_CYAN    "\e[46;1m"
#define ANSI_BG_BOLD_WHITE   "\e[47;1m"

// high intensity
#define ANSI_HI_BLACK   "\e[90m"
#define ANSI_HI_RED     "\e[91m"
#define ANSI_HI_GREEN   "\e[92m"
#define ANSI_HI_YELLOW  "\e[93m"
#define ANSI_HI_BLUE    "\e[94m"
#define ANSI_HI_MAGNETA "\e[95m"
#define ANSI_HI_CYAN    "\e[96m"
#define ANSI_HI_WHITE   "\e[97m"

// bright foreground colours
#define ANSI_HI_BOLD_BLACK   "\e[90;1m"
#define ANSI_HI_BOLD_RED     "\e[91;1m"
#define ANSI_HI_BOLD_GREEN   "\e[92;1m"
#define ANSI_HI_BOLD_YELLOW  "\e[93;1m"
#define ANSI_HI_BOLD_BLUE    "\e[94;1m"
#define ANSI_HI_BOLD_MAGNETA "\e[95;1m"
#define ANSI_HI_BOLD_CYAN    "\e[96;1m"
#define ANSI_HI_BOLD_WHITE   "\e[97;1m"


// Bright high intensity background
#define ANSI_HI_BG_BLACK  "\3[0;100m"
#define ANSI_HI_BG_RED    "\3[0;101m"
#define ANSI_HI_BG_GREEN  "\3[0;102m"
#define ANSI_HI_BG_YELLOW "\3[0;103m"
#define ANSI_HI_BG_BLUE   "\3[0;104m"
#define ANSI_HI_BG_PURPLE "\3[0;105m"
#define ANSI_HI_BG_CYAN   "\3[0;106m"
#define ANSI_HI_BG_WHITE  "\3[0;107m"

// Styling
#define ANSI_BOLD      "\e[1m"
#define ANSI_UNDERLINE "\e[4m"
#define ANSI_REVERSED  "\e[7m"


// Reset
#define ANSI_UP(n)         "\e[{" ## n ## "}A"
#define ANSI_DOWN         "\e[{" ## n ## "}B"
#define ANSI_RIGHT        "\e[{" ## n ##"}C"
#define ANSI_LEFT         "\e[{" ## n ##"}D"
#define ANSI_RESET        "\e[0m"
#define ANSI_CLEAR_SCREEN "\e[{" ## n ## "}J"
#define ANSI_CLEAR_LINE   "\e[{" ## n ## "}K"

#define ANSI(name)  puts(ANSI_##name)
#define ANSI_256(n) puts("\e[38;5;" # n "m")






#endif
