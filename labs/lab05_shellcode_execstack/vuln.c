#include <stdio.h>
#include <stdlib.h>

void setup() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

void vuln() {
    char buffer[128];

    printf("==========================================\n");
    printf("      🚀 LAB 05: SHELLCODE ON STACK        \n");
    printf("==========================================\n");
    // For beginners, we leak the stack buffer address so they don't have to guess ASLR offsets!
    printf("Helpful debug leak -> buffer is at: %p\n", buffer);
    printf("Enter your custom payload: ");

    gets(buffer); // Vulnerable!
    printf("Input received. Returning...\n");
}

int main() {
    setup();
    vuln();
    return 0;
}
