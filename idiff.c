#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include "list.h"

#define STD_BUFSIZE 256 /* max length of lines */
#define SEARCH_LEN 10 /* how many lines we will look ahead for insertions */

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

static void printline(char *left, char *right)
{
    struct winsize w;
    unsigned int l = 0;

    ioctl(0, TIOCGWINSZ, &w);

    fputs(" ", stdout);

    if (left) {
        l = strlen(left);
        if (left[l-1] == '\n')
            left[l-1] = ' ';
        fputs(left, stdout);
    }

    for (unsigned int i = 0; i < w.ws_col / 2 - l - 2; i++)
        fputs(" ", stdout);
    fputs("| ", stdout);

    if (right) {
        l = strlen(right);
        if (right[l-1] == '\n')
            right[l-1] = ' ';
        fputs(right, stdout);
    }

    fputs("\n", stdout);
}

static void compfiles(struct file fone, struct file ftwo)
{
    struct list_head *lone = fone.lines;
    struct list_head *ltwo = ftwo.lines;
    bool dupli = false, end = false;

    while (!end) {
        if(strcmp(lone->payload, ltwo->payload)) { /* no match */
            struct list_head *lonen = lone;
            struct list_head *ltwon = ltwo;

            /* try to figure out if we have additional lines added */
            for (unsigned int i = 0; i < SEARCH_LEN; i++) {
                if (lonen->next) {
                    lonen = lonen->next;
                    if (!strcmp(lonen->payload, ltwo->payload)) {
                        while (lone != lonen) {
                            printline(lone->payload, NULL);
                            lone = lone->next;
                        }
                        dupli = true;
                        break;
                    }
                }
                if (ltwon->next) {
                    ltwon = ltwon->next;
                    if (!strcmp(lone->payload, ltwon->payload)) {
                        while (ltwo != ltwon) {
                            printline(NULL, ltwo->payload);
                            ltwo = ltwo->next;
                        }
                        dupli = true;
                        break;
                    }
                }
            }

            if (!dupli)
                printline(lone->payload, ltwo->payload);
            dupli = false;
        }

        if (lone->next) {
            lone = lone->next;
        } else {
            end = true;
            while(ltwo->next) {
                ltwo = ltwo->next;
                printline(NULL, ltwo->payload);
            }
        }
        if (ltwo->next) {
            ltwo = ltwo->next;
        } else {
            end = true;
            while(lone->next) {
                lone = lone->next;
                printline(lone->payload, NULL);
            }
        }
    }
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

            printline(argv[i+1], argv[j+1]);
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

