#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STD_BUFSIZE 256

struct file {
    FILE *fp;
    char *buf;
    bool reading;
};

int
main(int argc, char *argv[])
{
    int retval = 0;
    struct file *files;
    bool done = false;

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
        files[i].buf = malloc(sizeof(char) * STD_BUFSIZE);
        if (!files[i].buf) {
            retval = ENOMEM;
            goto bufallocfail;
        }

        files[i].fp = fopen(argv[i+1], "r");
        if (files[i].fp == NULL) {
            retval = EIO;
            goto readfail;
        }
    }

    while (!done) {
        for (unsigned int i = 0; i < argc - 1; i++) {
            fgets(files[i].buf, STD_BUFSIZE, files[i].fp);
            /* TODO advance in lines and figure out when to stop on this file */
            if (!files[i].buf)
                files[i].reading = false;
        }

        /* TODO compare the lines */

        done = true;
        for (unsigned int i = 0; i < argc - 1; i++) {
            if (files[i].reading) {
                done = false;
                break;
            }
        }
    }

readfail:
bufallocfail:
    if (files) {
        for (unsigned int i = 0; i < argc - 1; i++) {
            if (files[i].fp)
                fclose(files[i].fp);
            if (files[i].buf)
                free(files[i].buf);
        }
        free(files);
    }
out:
    return retval;
}

