#include <stdio.h>

int main() {
    int liczba1, liczba2, suma;
    
    printf("Give me first number: ");
    scanf("%d", &liczba1);
    
    printf("Give me second number: ");
    scanf("%d", &liczba2);
    
    suma = liczba1 + liczba2;
    
    printf("Sum is: %d\n", suma);
    
    return 0;
}