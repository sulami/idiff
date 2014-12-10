#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STD_BUFSIZE 4096

int
main(int argc, char *argv[])
{
    char **files;
    int retval = 0;
    unsigned int maxsize = 0;

    if (argc < 3) {
        retval = EINVAL;
        goto out;
    }

    files = calloc(argc - 1, sizeof(char *));
    if (!files) {
        retval = ENOMEM;
        goto out;
    }

    for (unsigned int i = 0; i < argc - 1; i++) {
        FILE *fp;
        unsigned int bufsize = STD_BUFSIZE;
        unsigned int bufleft = STD_BUFSIZE;
        unsigned int read;

        files[i] = malloc(sizeof(char) * STD_BUFSIZE);
        if (!files[i]) {
            retval = ENOMEM;
            goto bufallocfail;
        }

        fp = fopen(argv[i+1], "r");
        if (!fp) {
            retval = EIO;
            goto readfail;
        }

        while ((read = fread(files[i], sizeof(char), bufleft, fp))) {
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

        fclose(fp);
    }

readfail:
bufallocfail:
    for (unsigned int i = 0; i < argc - 1; i++)
        if (files[i])
            free(files[i]);
    free(files);
out:
    return retval;
}

