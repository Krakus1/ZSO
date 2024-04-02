#include <stdio.h>
#include <stdlib.h> 

double** createCars() {
    // Alokuje pamięć dla tablicy 10x7
    double** cars = (double**)malloc(10 * sizeof(double*));
    for (int i = 0; i < 10; i++) {
        cars[i] = (double*)malloc(7 * sizeof(double));
    }

    // Wypełniam tablicę startowymi
    cars[0][0]=1;	cars[0][1]=100;	cars[0][2]=110; cars[0][3]=50; cars[0][4]=5; cars[0][5]=2.5; cars[0][6]=1.25;

    return cars;
}


int main() {
    double** cars = createCars();

    int i;
    printf("nrPojazdu|paliwo|smar|plynChlodniczy|zuzyciePaliwa|zuzycieSmaru|zuzyciePlynuChlodniczego|\n");
    for (i = 0; i < 1; i++) {
        printf("Pojazd numer %f: %f, %f, %f, %f, %f, %f\n", cars[i][0], cars[i][1], cars[i][2], cars[i][3], cars[i][4], cars[i][5], cars[i][6]);
    }

    for (i = 0; i< 1; i++)
    {
        cars[i][1] = cars[i][1] - cars[i][4];
        cars[i][2] = cars[i][2] - cars[i][5];
        cars[i][3] = cars[i][3] - cars[i][6];
    }

    printf("po pierwszej sekundzie jazdy:");
    printf("nrPojazdu|paliwo|smar|plynChlodniczy|zuzyciePaliwa|zuzycieSmaru|zuzyciePlynuChlodniczego|\n");
    for (i = 0; i < 1; i++) {
        printf("Pojazd numer %f: %f, %f, %f, %f, %f, %f\n", cars[i][0], cars[i][1], cars[i][2], cars[i][3], cars[i][4], cars[i][5], cars[i][6]);
    }

}