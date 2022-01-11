#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "solvesudoku.h"

char* createPuzzle(char* arrayPuzzle){
    int i, j,k=0;
    char* puzzle;

    puzzle = malloc(81);

    for (i = 0; i < 9; i++){
        for(j = 0; j < 9; j++){
            puzzle[i*9 + j] = arrayPuzzle[k++];
        }
    }
    return puzzle;
}

void printPuzzle(char* puzzle){
    int i, j, a;

    printf("\n");
    printf(" 0 | 1  2  3 | 4  5  6 | 7  8  9 | X\n");
    printf(" ---------------------------------\n");
    for (i = 0, a = 1; i < 9; i++, a++){
        for(j = 0; j < 9; j++){
            if (j == 0)
                printf(" %d |", a);
            else if ((j) % 3 == 0)
                printf("|");
            printf(" %c ", puzzle[i*9+j]);
            if (j == 8)
                printf("|");
        }
        printf("\n");
        if ((i + 1) % 3 == 0)
            printf(" ---------------------------------\n");
    }
    printf(" Y\n");
}

bool checkAvailable(char* puzzle, int* row, int* column){
    int i, j;

    for (i = 0; i < 9; i++){
        for(j = 0; j < 9; j++){
            if (puzzle[i*9+j] == 0+'0'){
                *row = i;
                *column  = j;
                return true;
            }
        }
    }

    return false;
}

bool checkBox(char* puzzle, int row, int column, int val){
    int i, j;
    int squareRow, squareColumn;

    //CHECK VERTICAL
    for(i = 0; i < 9; i++){
        if (puzzle[i*9+column] == val+'0')
            return false;
    }

    //CHECK HORIZONTAL
    for(j = 0; j < 9; j++){
        if (puzzle[row*9+j] == val+'0')
            return false;
    }

    //CHECK SQUARE
    squareRow = row - row % 3;
    squareColumn = column - column % 3;

    for(i = 0; i < 3; i++){
        for(j = 0; j < 3; j++){
            if(puzzle[(squareRow + i)*9+squareColumn + j] == val+'0')
                return false;
        }
    }

    return true;
}

bool solvePuzzle(char* puzzle){
    int i, j, val;

    if(!checkAvailable(puzzle, &i, &j))
        return true;

    for(val = 1; val < 10; val++){
        if(checkBox(puzzle, i, j, val+'0')){
            puzzle[i*9+j] = val+'0';
            if(solvePuzzle(puzzle))
                return true;
            else
                puzzle[i*9+j] = 0;
        }
    }
    return false;
}

char* copyPuzzle(char* puzzle){
    int i, j;
    char* newPuzzle;

    newPuzzle = malloc(81);
    for (i = 0; i < 9; i++){
        for(j = 0; j < 9; j++){
            newPuzzle[i*9+j] = puzzle[i*9+j];
        }
    }
    return newPuzzle;
}

void userChoice(char* userPuzzle, char* tempPuzzle, int positionY, int positionX, int userVal){
    int i,j;
    if(checkBox(userPuzzle, positionY, positionX, userVal))
        userPuzzle[positionY*9+positionX] = userVal+'0';
    else
        printf("\nincorrect value for the X = %d Y = %d coordinate, please try again\n",positionX + 1, positionY + 1);

    for (i = 0; i < 9; i++){
        for(j = 0; j < 9; j++){
            tempPuzzle[i*9+j] = userPuzzle[i*9+j];
        }
    }

    if(!solvePuzzle(tempPuzzle)){
        printf("\nincorrect value for the X = %d Y = %d coordinate, please try again\n",positionX + 1, positionY + 1);
        userPuzzle[positionY*9+positionX] = 0+'0';
    }
    getchar();
    printPuzzle(userPuzzle);

    return;
}
