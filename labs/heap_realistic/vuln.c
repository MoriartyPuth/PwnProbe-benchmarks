// Realistic menu-driven UAF: create prompts for index AND size, edit/delete/use
// prompt for an index. The old positional heap strategy (which sends a single
// input per action and no index to edit) cannot drive this; the prompt-driven
// strategy can. 64-bit, non-PIE, imports malloc/free. win() is name-recognized.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct obj { void (*fn)(void); char buf[24]; };
static struct obj *objs[16];

void win(void) { system("/bin/sh"); }
static void hello(void) { puts("hello"); }

static int menu(void) {
    int c;
    puts("1. create");
    puts("2. delete");
    puts("3. edit");
    puts("4. use");
    puts("5. exit");
    printf("> ");
    if (scanf("%d", &c) != 1) exit(0);
    return c;
}

int main(void) {
    setvbuf(stdout, 0, _IONBF, 0);
    setvbuf(stdin, 0, _IONBF, 0);
    int idx, sz;
    for (;;) {
        switch (menu()) {
        case 1:
            printf("index: "); if (scanf("%d", &idx) != 1) return 0;
            printf("size: ");  if (scanf("%d", &sz) != 1) return 0;
            getchar();
            if (idx < 0 || idx >= 16 || sz < 0 || sz > 4096) break;
            objs[idx] = malloc(sz);
            if (!objs[idx]) break;
            objs[idx]->fn = hello;
            printf("name: ");
            read(0, objs[idx]->buf, 23);
            break;
        case 2:
            printf("index: "); if (scanf("%d", &idx) != 1) return 0;
            getchar();
            if (idx >= 0 && idx < 16 && objs[idx]) free(objs[idx]); // UAF: not nulled
            break;
        case 3:
            printf("index: "); if (scanf("%d", &idx) != 1) return 0;
            getchar();
            if (idx < 0 || idx >= 16 || !objs[idx]) break;
            printf("data: ");
            read(0, objs[idx], 32); // overwrites fn pointer at offset 0
            break;
        case 4:
            printf("index: "); if (scanf("%d", &idx) != 1) return 0;
            getchar();
            if (idx >= 0 && idx < 16 && objs[idx]) objs[idx]->fn();
            break;
        default:
            return 0;
        }
    }
}
