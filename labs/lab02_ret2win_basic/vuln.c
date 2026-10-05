#include <stdio.h>
#include <stdlib.h>

void setup() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

// 🏆 Secret Vault: Never called by main()!
void win() {
    printf("\n🎉 BOOM! You hijacked RIP and returned to win()!\n");
    FILE *f = fopen("flag.txt", "r");
    if (f == NULL) {
        printf("FLAG{dummy_flag_replace_with_real_one_in_production}\n");
        return;
    }
    char ch;
    while ((ch = fgetc(f)) != EOF) {
        putchar(ch);
    }
    fclose(f);
    printf("\n");
    exit(0);
}

void vulnerable_function() {
    char buffer[32];
    printf("Please leave a note for the developers:\n> ");
    // gets() allows us to write past the 32-byte buffer and smash the return address!
    gets(buffer);
    printf("Thank you! Your note has been safely filed.\n");
}

int main() {
    setup();
    printf("==========================================\n");
    printf("       🎮 LAB 02: RET2WIN BASIC           \n");
    printf("==========================================\n");
    printf("Address of secret win() function: %p\n", win);
    vulnerable_function();
    printf("Program finished normally. Goodbye!\n");
    return 0;
}
