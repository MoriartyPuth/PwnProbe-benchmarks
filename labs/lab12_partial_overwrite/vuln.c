// PIE partial-overwrite with no leak. A read()-style overflow (no appended
// terminator) lets the solver overwrite only the low bytes of vuln()'s saved
// return address. Under PIE the low 12 bits are the fixed page offset, so a
// two-byte overwrite reaches win() within a 64 KiB window and the one remaining
// ASLR nibble is beaten by retrying across fresh runs. win() prints the flag.
// Build: gcc -O0 -fno-stack-protector   (PIE is the gcc default)
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
    char buf[64];
    printf("> ");
    read(0, buf, 128); // read() does not append a terminator
}

int main(void) {
    setup();
    vuln();
    return 0;
}
