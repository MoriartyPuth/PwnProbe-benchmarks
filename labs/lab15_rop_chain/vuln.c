// Chained multi-argument ROP: non-PIE, no leak, and no single function that
// prints the flag. Three gate functions must be called in order (one -> two ->
// three), each with the same three magic arguments, before the flag is printed;
// print_flag refuses until all three gates passed. So jumping to any one
// function does nothing -- only a chain of three 3-argument calls works, which is
// what the ROP planner builds. Mirrors ROP Emporium's `callme`, but
// self-contained (no shared library).
// Build: gcc -O0 -fno-stack-protector -no-pie
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int st1, st2, st3;

#define MA 0x1111111111111111UL
#define MB 0x2222222222222222UL
#define MC 0x3333333333333333UL

void print_flag(void) {
    if (!st3) return; // only after all three gates passed
    FILE *f = fopen("flag.txt", "r");
    if (!f) { puts("FLAG{dummy}"); return; }
    int ch;
    while ((ch = fgetc(f)) != EOF) putchar(ch);
    fclose(f);
    putchar('\n');
}

void check_one(unsigned long a, unsigned long b, unsigned long c) {
    if (a == MA && b == MB && c == MC) st1 = 1;
}
void check_two(unsigned long a, unsigned long b, unsigned long c) {
    if (st1 && a == MA && b == MB && c == MC) st2 = 1;
}
void check_three(unsigned long a, unsigned long b, unsigned long c) {
    if (st2 && a == MA && b == MB && c == MC) { st3 = 1; print_flag(); }
}

// glibc >= 2.34 dropped __libc_csu_init, the classic pop rdi;pop rsi;pop rdx;ret;
// provide it explicitly (older challenge binaries like callme ship one).
__attribute__((used)) void gadget(void) {
    __asm__ volatile("pop %rdi\n pop %rsi\n pop %rdx\n ret\n");
}

void setup(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
}

void vuln(void) {
    char buf[64];
    printf("> ");
    read(0, buf, 256); // overflow
}

int main(void) {
    setup();
    vuln();
    return 0;
}
