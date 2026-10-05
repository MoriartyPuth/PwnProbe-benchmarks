// Format-string GOT overwrite (non-PIE, partial RELRO). The user's input is used
// directly as a printf format string; a puts() call follows, whose GOT entry the
// solver overwrites with win() using %hhn byte writes. win() prints the flag, so
// the one-shot attempt discloses it.
// Build: gcc -O0 -fno-stack-protector -no-pie
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

int main(void) {
    setup();
    char buf[256];
    printf("Enter a message: ");
    if (!fgets(buf, sizeof buf, stdin)) return 0;
    printf(buf);   // vulnerable: user-controlled format string
    puts("Goodbye"); // puts GOT entry is called after the vulnerable printf
    exit(0);         // exit GOT entry: another overwrite target reached after printf
}
