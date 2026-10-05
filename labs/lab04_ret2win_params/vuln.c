#include <stdio.h>
#include <stdlib.h>

void setup() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

// Helper function providing handy ROP gadgets for students
void gadget_shop() {
    __asm__(
        "pop %rdi; ret;\n"
        "pop %rsi; ret;\n"
    );
}

// 🏆 Secret function requiring two specific keys!
void win(unsigned long key1, unsigned long key2) {
    printf("\n[Checking Keys] Key 1: 0x%lx | Key 2: 0x%lx\n", key1, key2);

    if (key1 == 0x1337BEEF && key2 == 0xCAFEBABE) {
        printf("🎉 EXCELLENT! Both parameters matched!\n");
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
    } else {
        printf("❌ Access Denied: Incorrect keys provided.\n");
        printf("Hint: Key 1 must be in RDI (0x1337BEEF), Key 2 in RSI (0xCAFEBABE)!\n");
        exit(1);
    }
}

void vuln() {
    char secret_phrase[32];
    printf("Please enter the secret master passphrase: ");
    gets(secret_phrase); // Vulnerable!
}

int main() {
    setup();
    printf("==========================================\n");
    printf("     🔐 LAB 04: RET2WIN WITH PARAMETERS    \n");
    printf("==========================================\n");
    vuln();
    printf("Exiting without unlocking.\n");
    return 0;
}
