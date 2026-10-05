#include <stdio.h>
#include <stdlib.h>

void setup() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

// Gadget shop for beginners
void gadgets() {
    __asm__("pop %rdi; ret;");
}

void vuln() {
    char feedback[32];
    printf("Please enter your survey feedback: ");
    gets(feedback); // Vulnerability!
}

int main() {
    setup();

    printf("==========================================\n");
    printf("       🏛️ LAB 07: RET2LIBC (DEFEAT NX)    \n");
    printf("==========================================\n");
    printf("Notice: NX is ENABLED! Stack shellcode will fail.\n");
    printf("There is NO win() function in this program.\n");
    // Leak system() address to bypass ASLR easily in this intro lab!
    printf("System library leak -> system() is at: %p\n", system);

    vuln();

    printf("Survey complete. Goodbye!\n");
    return 0;
}
