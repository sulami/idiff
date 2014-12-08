#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define STD_BUFSIZE 128

int
main(int argc, char *argv[])
{
    int **files;
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

        files[i] = calloc(sizeof(char), STD_BUFSIZE);
        if (!files[i]) {
            retval = ENOMEM;
            goto bufallocfail;
        }

        fp = fopen(argv[i+1], "r");
        if (!fp) {
            retval = EIO;
            goto readfail;
        }
        fclose(fp);
    }

readfail:
bufallocfail:
    for (int i = 0; i < argc - 1; i++)
        if (files[i])
            free((int *)files[i]);
    free(files);
out:
    return retval;
}

