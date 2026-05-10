#include <stdio.h>
#include <string.h>

void GenerateKey(const char* username, char* output)
{
    char buffer[32] = "_r <()<1-Z2[l5,^";

    int userLen = strlen(username);
    int seedLen = strlen(buffer);
    int maxLen = (userLen > seedLen) ? userLen : seedLen;

    for (int i = 0; i < maxLen; i++)
    {
        unsigned char userChar = username[i % userLen];
        unsigned char seedChar = buffer[i % seedLen];

        buffer[i % seedLen] = ((userChar ^ seedChar) % 25) + 'A';
    }

    sprintf(output, "%.4s-%.4s-%.4s-%.4s",
        &buffer[0], &buffer[4], &buffer[8], &buffer[12]);
}

int main(void)
{
    char username[64];
    char serial[64];

    printf("Enter username: ");
    scanf("%s", username);

    GenerateKey(username, serial);

    printf("Reg. Code: %s\n", serial);

    getchar();

    printf("Press Enter to exit...");
    getchar();

    return 0;
}
