#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setup() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

int main() {
    setup();

    char secret_flag[64] = {0};
    char user_input[128];

    // Load secret flag into a local stack variable
    FILE *f = fopen("flag.txt", "r");
    if (f != NULL) {
        fgets(secret_flag, sizeof(secret_flag), f);
        fclose(f);
    } else {
        strcpy(secret_flag, "FLAG{dummy_test_flag_format_string_9901}");
    }

    printf("==========================================\n");
    printf("   🔮 LAB 06: FORMAT STRING PEEKING GLASS \n");
    printf("==========================================\n");
    printf("Notice: There is NO buffer overflow here! (fgets bounds input to 128 bytes)\n");
    printf("Can you still read the secret flag sitting on the stack?\n\n");
    printf("What is your name? ");

    // Safe read: bounds check prevents classic buffer overflow!
    fgets(user_input, sizeof(user_input), stdin);

    printf("Greeting: ");
    // VULNERABILITY: User-controlled format string!
    printf(user_input);

    printf("\nSession closed. Goodbye!\n");
    return 0;
}
