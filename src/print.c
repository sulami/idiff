#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include "idiff.h"

void
printline(char *left, char *right)
{
    struct winsize w;
    int l = 0;

    ioctl(0, TIOCGWINSZ, &w);

    fputs(" ", stdout);

    if (left) {
#if USE_COLOUR
        if (right)
            fputs(CL_YELLOW, stdout);
        else
            fputs(CL_RED, stdout);
#endif
        l = strlen(left);
        if (l && left[l-1] == '\n')
            left[l-1] = ' ';
        fputs(left, stdout);
#if USE_COLOUR
        fputs(CL_DEFAULT, stdout);
#endif
    }

    if (l && l <= w.ws_col / 2 - 2) {
        for (int i = 0; i < w.ws_col / 2 - 2 - l; i++)
            fputs(" ", stdout);
        fputs("| ", stdout);
    } else {
        if (l)
            fputs("\n", stdout);
        for (int i = 0; i < w.ws_col / 2 - 2; i++)
            fputs(" ", stdout);
        fputs("| ", stdout);
    }

    if (right) {
#if USE_COLOUR
        if (left)
            fputs(CL_YELLOW, stdout);
        else
            fputs(CL_GREEN, stdout);
#endif
        l = strlen(right);
        if (l && right[l-1] == '\n')
            right[l-1] = ' ';
        fputs(right, stdout);
#if USE_COLOUR
        fputs(CL_DEFAULT, stdout);
#endif
    }

    fputs("\n", stdout);
}

