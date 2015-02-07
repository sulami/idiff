#ifndef IDIFF_H
#define IDIFF_H

#include <stdbool.h>
#include <stdio.h>

#define STD_BUFSIZE 256 /* max length of lines */
#define SEARCH_LEN 10 /* how many lines we will look ahead for insertions */
#define USE_COLOUR 1 /* use colours */

#if USE_COLOUR
#define CL_DEFAULT "\033[0m"
#define CL_RED     "\033[91m"
#define CL_GREEN   "\033[92m"
#define CL_YELLOW  "\033[93m"
#endif

struct file {
    FILE *fp;
    struct list_head *lines;
    bool reading;
};

#endif /* IDIFF_H */

