// Logic/stdin puzzle solved by symbolic execution (angr), not memory corruption.
// A fixed key gates print_flag(); angr finds the stdin that reaches print_flag
// (a name it targets), and PwnProbe replays that input one-shot and keeps the
// flag only because it actually prints. No overflow, no leak.
// Build: gcc -O0   (any protections are fine; this is a logic check)
// Needs PWNPROBE_PYTHON pointing at a Python with angr installed.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setup(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

void print_flag(void) {
    FILE *f = fopen("flag.txt", "r");
    if (!f) { puts("FLAG{dummy}"); return; }
    char ch;
    while ((ch = fgetc(f)) != EOF) putchar(ch);
    fclose(f);
    putchar('\n');
}

int main(void) {
    setup();
    char buf[64];
    printf("Enter the key: ");
    if (!fgets(buf, sizeof buf, stdin)) return 0;
    buf[strcspn(buf, "\n")] = 0;
    if (strcmp(buf, "op3n_s3sam3") == 0)
        print_flag();
    else
        puts("Access denied.");
    return 0;
}
