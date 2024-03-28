#include <stdio.h>

int main() {
    
    // 1 - fuel
    // 2 - lubricant
    // 3 - cooled_liquid
    // 4 
double cars[10][7];
//nrPojazdu|paliwo|smar|plynChlodniczy|zuzyciePaliwa|zuzycieSmaru|zuzyciePlynuChlodniczego|
    cars[0][0]=1;	cars[0][1]=100;	cars[0][2]=110; cars[0][3]=50; cars[0][4]=5; cars[0][5]=2.5; cars[0][6]=1.25;
    // cars[1][0]=8;	cars[1][1]=10;	cars[1][2]=6;
    // cars[2][0]=5;	cars[2][1]=10;	cars[2][2]=7;
    // cars[3][0]=6;	cars[3][1]=5;	cars[3][2]=7;
    // cars[4][0]=7;	cars[4][1]=7;	cars[4][2]=5;
    // cars[5][0]=7;	cars[5][1]=8;	cars[5][2]=5;
    // cars[6][0]=8;	cars[6][1]=6;	cars[6][2]=8;
    // cars[7][0]=8;	cars[7][1]=6;	cars[7][2]=9;
    // cars[8][0]=8;	cars[8][1]=5;	cars[8][2]=9;
    // cars[9][0]=6;	cars[9][1]=6;	cars[9][2]=9;
    // cars[10][0]=10;	cars[10][1]=7;	cars[10][2]=8;
    // cars[11][0]=5;	cars[11][1]=9;	cars[11][2]=7;
    // cars[12][0]=6;	cars[12][1]=7;	cars[12][2]=5;
    // cars[13][0]=7;	cars[13][1]=5;	cars[13][2]=5;
    // cars[14][0]=5;	cars[14][1]=9;	cars[14][2]=5;
    // cars[15][0]=5;	cars[15][1]=10;	cars[15][2]=6;
    // cars[16][0]=9;	cars[16][1]=7;	cars[16][2]=7;
    // cars[17][0]=5;	cars[17][1]=8;	cars[17][2]=7;
    // cars[18][0]=10;	cars[18][1]=8;	cars[18][2]=5;
    // cars[19][0]=6;	cars[19][1]=8;	cars[19][2]=6;
    // cars[20][0]=5;	cars[20][1]=8;	cars[20][2]=9;

    // Wypełnienie tablicy przykładowymi danymi
     int i, j;

    // Wyświetlenie zawartości tablicy
    printf("Value in array:\n");
    for (i = 0; i < 1; i++) {
        printf("Pozycja %f: %f, %f, %f, %f, %f, %f\n", cars[i][0], cars[i][1], cars[i][2], cars[i][3], cars[i][4], cars[i][5], cars[i][6]);
    }
    
    return 0;
}
