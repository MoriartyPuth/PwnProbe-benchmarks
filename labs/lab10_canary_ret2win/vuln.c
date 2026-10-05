// Canary-bypass ret2win (non-PIE, stack canary enabled). vuln() first leaks the
// stack through a format string (disclosing the canary), then takes an
// overflowing read past the canary. In one connection the solver leaks the
// canary live and sends padding + canary + saved rbp + ret2win so the stack
// check passes and win() runs. win() prints the flag.
// Build: gcc -O0 -no-pie -fstack-protector-all
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void setup(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

void win(void) {
    FILE *f = fopen("flag.txt", "r");
    if (!f) { puts("FLAG{dummy}"); return; }
    char ch;
    while ((ch = fgetc(f)) != EOF) putchar(ch);
    fclose(f);
    putchar('\n');
}

void vuln(void) {
    char leakbuf[128];
    char buf[64];
    printf("Name? ");
    if (!fgets(leakbuf, sizeof leakbuf, stdin)) return;
    printf(leakbuf);   // format-string leak (discloses the canary)
    printf("Data? ");
    read(0, buf, 512); // overflow past the canary
}

int main(void) {
    setup();
    vuln();
    return 0;
}
