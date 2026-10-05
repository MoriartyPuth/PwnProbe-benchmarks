#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Disable buffering so outputs appear immediately over netcat/pipes
void setup() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

void print_flag() {
    printf("\n🎉 ACCESS GRANTED! You overwrote the authorization variable!\n");
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
}

int main() {
    setup();

    // The target variable: 0 = regular user, 0x1337 = Super Admin!
    volatile int auth_level = 0;
    char username[32];

    printf("==========================================\n");
    printf("   🛡️  WELCOME TO ACME SECURE LOGIN v1.0  \n");
    printf("==========================================\n");
    printf("Current auth_level: 0x%08x (Need 0x1337BEEF)\n", auth_level);
    printf("Enter your username: ");

    // Vulnerable function: gets() reads without bounds checking!
    gets(username);

    printf("\nLogin attempt completed for: %s\n", username);
    printf("Resulting auth_level: 0x%08x\n", auth_level);

    if (auth_level == 0x1337BEEF) {
        print_flag();
    } else if (auth_level != 0) {
        printf("⚠️ auth_level was modified (0x%08x), but didn't match 0x1337BEEF!\n", auth_level);
        printf("Check your byte ordering (Little-Endian) and try again!\n");
    } else {
        printf("❌ Access Denied: Regular user privileges only.\n");
    }

    return 0;
}
