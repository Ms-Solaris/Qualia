#include <stdio.h>

int main() {
    FILE *fptr;
    fptr = fopen("Test.txt", "r");

    char firstWords[50];
    fgets(firstWords, 50, fptr);
    printf("%s", firstWords);
    fclose(fptr);
}