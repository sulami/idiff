#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define STD_BUFSIZE 256 /* max length of lines */

struct file {
    FILE *fp;
    struct list_head *lines;
    bool reading;
};

static void readline(struct file *f)
{
    char *line;

    line = calloc(STD_BUFSIZE, sizeof(char));
    if (!line)
        return;

    f->lines = list_add(f->lines, line);
    f->reading = fgets(line, STD_BUFSIZE, f->fp) ? true : false;
}

int main(int argc, char *argv[])
{
    int retval = 0;
    struct file *files;

    if (argc < 3) {
        retval = EINVAL;
        goto out;
    }

    files = malloc((argc - 1) * sizeof(struct file));
    if (!files) {
        retval = ENOMEM;
        goto out;
    }

    for (unsigned int i = 0; i < argc - 1; i++) {
        files[i].reading = true;
        files[i].lines = NULL;

        files[i].fp = fopen(argv[i+1], "r");
        if (files[i].fp == NULL) {
            retval = EIO;
            goto readfail;
        }
    }

    for (unsigned int i = 0; i < argc - 1; i++)
        while (files[i].reading)
            readline(&files[i]);

    /* TODO compare the lines */

readfail:
    if (files) {
        for (unsigned int i = 0; i < argc - 1; i++) {
            if (files[i].fp)
                fclose(files[i].fp);
            list_delete(files[i].lines);
        }
        free(files);
    }
out:
    return retval;
}

