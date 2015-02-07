#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "idiff.h"
#include "list.h"
#include "print.h"

void
compfiles(struct file fone, struct file ftwo)
{
    struct list_head *lone = fone.lines;
    struct list_head *ltwo = ftwo.lines;
    bool dupli = false, end = false;

    while (!end) {
        if(strcmp(lone->payload, ltwo->payload)) { /* no match */
            struct list_head *lonen = lone;
            struct list_head *ltwon = ltwo;

            /* try to figure out if we have additional lines added */
            for (int i = 0; i < SEARCH_LEN; i++) {
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

