#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STD_BUFSIZE 128

int
main(int argc, char *argv[])
{
    char ***files;
    int retval = 0;

    if (argc < 3) {
        retval = EINVAL;
        goto out;
    }

    files = calloc(argc - 1, sizeof(int *));
    if (!files) {
        retval = ENOMEM;
        goto out;
    }

    for (int i = 0; i < argc - 1; i++) {
        FILE *fp;
        int bufsize = STD_BUFSIZE;
        int bufleft = STD_BUFSIZE;

        files[i] = malloc(sizeof(char *) * STD_BUFSIZE);
        if (!files[i]) {
            retval = ENOMEM;
            goto bufallocfail;
        }

        *files[i] = malloc(sizeof(char) * STD_BUFSIZE);
        if (!*files[i]) {
            retval = ENOMEM;
            goto bufallocfail;
        }

        fp = fopen(argv[i+1], "r");
        if (!fp) {
            retval = EIO;
            goto readfail;
        }

        while (fgets(*files[i], bufsize, fp)) {
            if (bufleft < 2) {
                bufsize += STD_BUFSIZE;
                *files[i] = realloc(*files[i], bufsize);
                if (!*files[i]) {
                    retval = ENOMEM;
                    goto bufallocfail;
                }
            }
            printf("%s", *files[i]);
            bufleft -= strlen(*files[i]);
        }

        fclose(fp);
    }

readfail:
bufallocfail:
    for (int i = 0; i < argc - 1; i++) {
        if (files[i]) {
            if (*files[i])
                free(*files[i]);
            free(files[i]);
        }
    }
    free(files);
out:
    return retval;
}

