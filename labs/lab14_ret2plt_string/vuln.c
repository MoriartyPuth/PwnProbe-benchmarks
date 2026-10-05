// ret2plt: non-PIE, no leak, and crucially NO function that prints the flag.
// The overflow must itself call system() with a pointer to the "/bin/cat
// flag.txt" string in the binary, using a `pop rdi; ret` gadget and system@plt.
// The decoy only ever runs "/bin/ls", so jumping to it does not help -- the flag
// is reachable only by composing the gadget + string + system (exactly ROP
// Emporium's `split`), which is what the ret2plt strategy does.
// Build: gcc -O0 -fno-stack-protector -no-pie
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// The flag-reading command lives in the binary but is never passed to system()
// by any function; only a composed ROP chain can use it.
char cmd[] = "/bin/cat flag.txt";

// Links system@plt, but with a harmless argument, so jumping here leaks nothing.
__attribute__((used)) void decoy(void) {
    system("/bin/ls");
}

// glibc >= 2.34 dropped __libc_csu_init, the classic source of `pop rdi; ret`,
// so modern -no-pie binaries often lack it. This provides the gadget the chain
// needs (older challenge binaries like ROP Emporium's split still ship it).
__attribute__((used)) void gadget(void) {
    __asm__ volatile("pop %rdi\n ret\n");
}

void setup(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
}

void vuln(void) {
    char buf[64];
    printf("Give me your input: ");
    read(0, buf, 256); // overflow; read() keeps arbitrary bytes
}

int main(void) {
    setup();
    vuln();
    return 0;
}
