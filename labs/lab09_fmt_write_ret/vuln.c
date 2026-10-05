// Format-string write to a saved return address, defeating full RELRO (read-only
// GOT). The vulnerable printf is in vuln(), called in a loop, so the solver can
// leak the stack and then, in the same connection, write win()'s low bytes over
// vuln()'s saved return address (which points back into main, sharing win()'s
// high bytes). On EOF the loop ends and vuln() returns into win().
// Build: gcc -O0 -fno-stack-protector -no-pie -Wl,-z,relro,-z,now
#include <stdio.h>
#include <stdlib.h>

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
    char buf[256];
    printf("> ");
    if (!fgets(buf, sizeof buf, stdin)) exit(0);
    printf(buf); // vulnerable: user-controlled format string, in a loop
}

int main(void) {
    setup();
    for (int i = 0; i < 64; i++) vuln();
    return 0;
}
