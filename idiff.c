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

    files = calloc(sizeof(int *), (argc - 1));
    if (!files) {
        retval = ENOMEM;
        goto out;
    }

    for (int i = 0; i < argc - 1; i++) {
        FILE *fp;

        files[i] = calloc(STD_BUFSIZE, sizeof(char));
        if (!files[i]) {
            retval = ENOMEM;
            goto bufallocfail;
        }

        fp = fopen(argv[i], "r");
        fclose(fp);
    }

bufallocfail:
    for (int i = 0; i < argc - 1; i++) {
        if (files[i])
            free((int *)files[i]);
    }
    free(files);
out:
    return retval;
}

