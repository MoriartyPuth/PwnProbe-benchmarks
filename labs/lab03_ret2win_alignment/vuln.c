#include <stdio.h>
#include <stdlib.h>

void setup() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

// 🏆 win() calls system() directly.
// On 64-bit Linux, system() requires RSP to be aligned to 16 bytes!
void win() {
    printf("\n🎉 Landing in win()! Calling system(\"cat flag.txt\")...\n");
    system("cat flag.txt");
    exit(0);
}

void vuln() {
    char name[64];
    printf("Welcome to the 64-bit Alignment Gauntlet!\n");
    printf("What is your gamer tag? ");
    gets(name); // Vulnerable!
    printf("Nice to meet you, %s!\n", name);
}

int main() {
    setup();
    vuln();
    return 0;
}
