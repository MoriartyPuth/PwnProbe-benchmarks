// ret2syscall (statically linked, non-PIE). A gets() overflow lets the solver
// build an execve("/bin/sh", 0, 0) ROP chain from the pop rax/rdi/rsi/rdx; ret
// gadgets, the syscall instruction, and the "/bin/sh" string that a static glibc
// provides. No libc resolution is needed; the spawned shell is driven to print
// the flag.
// Build: gcc -O0 -static -no-pie -fno-stack-protector
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// A static glibc may garbage-collect the "/bin/sh" string and the exact register
// gadgets when nothing references them. This global string and gadget block
// guarantee the pieces a classic ret2syscall challenge exposes:
//   pop rax;ret  pop rdi;ret  pop rsi;ret  pop rdx;ret  syscall;ret
char binsh[] = "/bin/sh";

__attribute__((used)) void gadgets(void) {
    __asm__ volatile(
        "pop %rax\n ret\n"
        "pop %rdi\n ret\n"
        "pop %rsi\n ret\n"
        "pop %rdx\n ret\n"
        "syscall\n ret\n");
}

void setup(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

void vuln(void) {
    char buf[64];
    printf("Echo: ");
    read(0, buf, 256); // overflow (read keeps newline bytes, unlike gets)
}

int main(int argc, char **argv) {
    setup();
    if (argc > 99) { gadgets(); puts(binsh); } // keep the symbols referenced
    vuln();
    return 0;
}
