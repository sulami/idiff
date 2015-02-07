#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "diff.h"
#include "idiff.h"
#include "list.h"
#include "print.h"

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

    for (int i = 0; i < argc - 1; i++) {
        files[i].reading = true;
        files[i].lines = NULL;

        files[i].fp = fopen(argv[i+1], "r");
        if (files[i].fp == NULL) {
            retval = EIO;
            goto readfail;
        }
    }

    for (int i = 0; i < argc - 1; i++)
        while (files[i].reading)
            readline(&files[i]);

    for (int i = 0; i < argc - 2; i++) {
        for (int j = i + 1; j < argc - 1; j++) {
            struct file fone = files[i];
            struct file ftwo = files[j];

            printline(argv[i+1], argv[j+1]);
            compfiles(fone, ftwo);
        }
    }

readfail:
    if (files) {
        for (int i = 0; i < argc - 1; i++) {
            if (files[i].fp)
                fclose(files[i].fp);
            list_delete(files[i].lines);
        }
        free(files);
    }
out:
    return retval;
}

