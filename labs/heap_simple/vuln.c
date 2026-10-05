// Minimal UAF matching the ORIGINAL positional heap strategy: create takes a
// single data input (no size/index), delete takes an index, edit writes raw
// bytes with NO index prompt (single implicit slot 0), use takes an index.
// Kept so the generalized strategy is checked against the old shape too.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct obj { void (*fn)(void); char buf[24]; };
static struct obj *obj0;

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
    getchar();
    return c;
}

int main(void) {
    setvbuf(stdout, 0, _IONBF, 0);
    setvbuf(stdin, 0, _IONBF, 0);
    int idx;
    for (;;) {
        switch (menu()) {
        case 1:
            obj0 = malloc(sizeof(struct obj));
            obj0->fn = hello;
            printf("name: ");
            read(0, obj0->buf, 23);
            break;
        case 2:
            printf("index: "); if (scanf("%d", &idx) != 1) return 0;
            getchar();
            (void)idx;
            free(obj0); // UAF: not nulled
            break;
        case 3:
            printf("data: ");
            read(0, obj0, 32); // overwrites fn pointer, no index prompt
            break;
        case 4:
            printf("index: "); if (scanf("%d", &idx) != 1) return 0;
            getchar();
            (void)idx;
            obj0->fn();
            break;
        default:
            return 0;
        }
    }
}
