#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define STD_BUFSIZE 256 /* max length of lines */

#define MAX(x,y) (x >= y ? x : y)

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

static int compfiles(struct file fone, struct file ftwo)
{
    struct list_head *lone = fone.lines;
    struct list_head *ltwo = ftwo.lines;
    bool end = false;

    while (!end) {
        if(!strcmp(lone->payload, ltwo->payload)) { /* match */
            if (lone->next)
                lone = lone->next;
            else /* TODO print rest */
                end = true;
            if (ltwo->next)
                ltwo = ltwo->next;
            else /* TODO print rest */
                end = true;
        } else { /* no match */
            unsigned int len = MAX(list_length(lone), list_length(ltwo));
            bool found = false;
            struct list_head *lonen = lone;
            struct list_head *ltwon = ltwo;

            /* try to figure out if we have additional lines added */
            for (unsigned int i = 0; i < len; i++) {
                if (i < list_length(lone) - 1) {
                    lonen = lonen->next;
                    if (!strcmp(lonen->payload, ltwo->payload)) {
                        lone = lonen;
                        found = true;
                        break;
                    }
                }
                if (i < list_length(ltwo) - 1) {
                    ltwon = ltwon->next;
                    if (!strcmp(lone->payload, ltwon->payload)) {
                        ltwo = ltwon;
                        found = true;
                        break;
                    }
                }
            }

            if (!found) { /* advance both lines */
                /* TODO print both lines */
                if (lone->next)
                    lone = lone->next;
                else /* print rest */
                    end = true;
                if (ltwo->next)
                    ltwo = ltwo->next;
                else /* print rest */
                    end = true;
            }
        }
    }

    return 0;
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

    for (unsigned int i = 0; i < argc - 2; i++) {
        for (unsigned int j = i + 1; j < argc - 1; j++) {
            struct file fone = files[i];
            struct file ftwo = files[j];

            compfiles(fone, ftwo);
        }
    }

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

