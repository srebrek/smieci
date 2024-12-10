#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    char* str1 = (char*)malloc(strlen(argv[1]) * sizeof(char));
    if (!str1) return 0; //err
    strcpy(str1, argv[1]);
    printf("poczatkowy napis: %s\n", str1);
    char buffer;
    for (int i = 0; i < strlen(str1); i++)
    {
        if (islower(str1[i]))
        {
            buffer = str1[i] - 'a';
            buffer = (buffer + 3) % 26;
            buffer += 'a';
            str1[i] = buffer;
        }
        else if (isupper(str1[i]))
        {
            buffer = str1[i] - 'A';
            buffer = (buffer + 3) % 26;
            buffer += 'A';
            str1[i] = buffer;
        }
        else
        {
            ;
        }
    }
    printf("zakodowany napis: %s\n", str1);
    
}
