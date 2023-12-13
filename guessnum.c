#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void){
    srand(time(NULL));
    printf("Guess the number!\n");
    int num = (int) floor((double) rand() / RAND_MAX * 100.0);
    int guess;
    do {
        scanf("%d", &guess);
        if(guess>num){
            printf("Lower!\n");
            continue;
        }
        printf("Higher!\n");
    } while(guess!=num);
    printf("You won!\n");
    return 0;
}
