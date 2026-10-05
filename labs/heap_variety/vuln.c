// Different menu vocabulary (Allocate/Release/Modify/Trigger), a size prompt, an
// index ("slot") prompt, and the function pointer at offset 8 (not 0) so the
// strategy must both recognise the synonyms and sweep the fn-ptr offset.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct obj { char tag[8]; void (*fn)(void); char buf[16]; };
static struct obj *objs[16];

void get_shell(void) { system("/bin/sh"); } // name-recognized win
static void hello(void) { puts("hello"); }

static int menu(void) {
    int c;
    puts("1. Allocate");
    puts("2. Release");
    puts("3. Modify");
    puts("4. Trigger");
    puts("5. Quit");
    printf("choice: ");
    if (scanf("%d", &c) != 1) exit(0);
    return c;
}

int main(void) {
    setvbuf(stdout, 0, _IONBF, 0);
    setvbuf(stdin, 0, _IONBF, 0);
    int slot, bytes;
    for (;;) {
        switch (menu()) {
        case 1:
            printf("slot: ");  if (scanf("%d", &slot) != 1) return 0;
            printf("bytes: "); if (scanf("%d", &bytes) != 1) return 0;
            getchar();
            if (slot < 0 || slot >= 16 || bytes < 0 || bytes > 4096) break;
            objs[slot] = malloc(bytes);
            if (!objs[slot]) break;
            objs[slot]->fn = hello;
            printf("content: ");
            read(0, objs[slot]->buf, 15);
            break;
        case 2:
            printf("slot: "); if (scanf("%d", &slot) != 1) return 0;
            getchar();
            if (slot >= 0 && slot < 16 && objs[slot]) free(objs[slot]); // UAF
            break;
        case 3:
            printf("slot: "); if (scanf("%d", &slot) != 1) return 0;
            getchar();
            if (slot < 0 || slot >= 16 || !objs[slot]) break;
            printf("content: ");
            read(0, objs[slot], 32); // overwrites fn at offset 8
            break;
        case 4:
            printf("slot: "); if (scanf("%d", &slot) != 1) return 0;
            getchar();
            if (slot >= 0 && slot < 16 && objs[slot]) objs[slot]->fn();
            break;
        default:
            return 0;
        }
    }
}
