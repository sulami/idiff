#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STD_BUFSIZE 4096

int
main(int argc, char *argv[])
{
    FILE **fps;
    char **files;
    int retval = 0;
    unsigned int maxsize = 0;

    if (argc < 3) {
        retval = EINVAL;
        goto out;
    }

    fps = calloc(argc - 1, sizeof(FILE *));
    if (!fps) {
        retval = ENOMEM;
        goto out;
    }

    files = calloc(argc - 1, sizeof(char *));
    if (!files) {
        retval = ENOMEM;
        goto fpallocfail;
    }

    for (unsigned int i = 0; i < argc - 1; i++) {
        files[i] = malloc(sizeof(char) * STD_BUFSIZE);
        if (!files[i]) {
            retval = ENOMEM;
            goto bufallocfail;
        }

        fps[i] = fopen(argv[i+1], "r");
        if (fps[i] == NULL) {
            retval = EIO;
            goto readfail;
        }
    }

    for (unsigned int i = 0; i < argc - 1; i++) {
        unsigned int bufsize = STD_BUFSIZE;
        unsigned int bufleft = STD_BUFSIZE;
        unsigned int read;

        while ((read = fread(files[i], sizeof(char), bufleft, fps[i]))) {
            bufleft -= read;
            if (!bufleft) {
                bufsize += STD_BUFSIZE;
                bufleft += STD_BUFSIZE;
                files[i] = realloc(files[i], sizeof(char) * bufsize);
                if (!files[i]) {
                    retval = ENOMEM;
                    goto bufallocfail;
                }
            }
            if (bufsize > maxsize)
                maxsize = bufsize;
        }
    }

readfail:
bufallocfail:
    for (unsigned int i = 0; i < argc - 1; i++) {
        if (fps[i] != NULL)
            fclose(fps[i]);
        if (files[i])
            free(files[i]);
    }
    free(files);
fpallocfail:
    free(fps);
out:
    return retval;
}

