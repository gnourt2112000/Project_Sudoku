#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "solvesudoku.h"

int** createPuzzle(char* arrayPuzzle){
    int i, j,k=0;
    int** puzzle;

    puzzle = (int**)malloc(sizeof(int*) * 9);

    for (i = 0; i < 9; i++){
        puzzle[i] = (int*)malloc(sizeof(int) * 9);
        for(j = 0; j < 9; j++){
            puzzle[i][j] = arrayPuzzle[k++]-48;
        }
    }
    return puzzle;
}

void printPuzzle(int** puzzle){
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
            printf(" %d ", puzzle[i][j]);
            if (j == 8)
                printf("|");
        }
        printf("\n");
        if ((i + 1) % 3 == 0)
            printf(" ---------------------------------\n");
    }
    printf(" Y\n");
}

bool checkAvailable(int** puzzle, int* row, int* column){
    int i, j;

    for (i = 0; i < 9; i++){
        for(j = 0; j < 9; j++){
            if (puzzle[i][j] == 0){
                *row = i;
                *column  = j;
                return true;
            }
        }
    }

    return false;
}

bool checkBox(int** puzzle, int row, int column, int val){
    int i, j;
    int squareRow, squareColumn;

    //CHECK VERTICAL
    for(i = 0; i < 9; i++){
        if (puzzle[i][column] == val)
            return false;
    }

    //CHECK HORIZONTAL
    for(j = 0; j < 9; j++){
        if (puzzle[row][j] == val)
            return false;
    }

    //CHECK SQUARE
    squareRow = row - row % 3;
    squareColumn = column - column % 3;

    for(i = 0; i < 3; i++){
        for(j = 0; j < 3; j++){
            if(puzzle[squareRow + i][squareColumn + j] == val)
                return false;
        }
    }

    return true;
}

bool solvePuzzle(int** puzzle){
    int i, j, val;

    if(!checkAvailable(puzzle, &i, &j))
        return true;

    for(val = 1; val < 10; val++){
        if(checkBox(puzzle, i, j, val)){
            puzzle[i][j] = val;
            if(solvePuzzle(puzzle))
                return true;
            else
                puzzle[i][j] = 0;
        }
    }
    return false;
}

int** copyPuzzle(int** puzzle){
    int i, j;
    int** newPuzzle;

    newPuzzle = (int**)malloc(sizeof(int*) * 9);
    for (i = 0; i < 9; i++){
        newPuzzle[i] = (int*)malloc(sizeof(int) * 9);
        for(j = 0; j < 9; j++){
            newPuzzle[i][j] = puzzle[i][j];
        }
    }
    return newPuzzle;
}

void userChoice(int** userPuzzle, int** tempPuzzle, int positionY, int positionX, int userVal){
    int i,j;
    if(checkBox(userPuzzle, positionY, positionX, userVal))
        userPuzzle[positionY][positionX] = userVal;
    else
        printf("\nincorrect value for the X = %d Y = %d coordinate, please try again\n",positionX + 1, positionY + 1);

    for (i = 0; i < 9; i++){
        for(j = 0; j < 9; j++){
            tempPuzzle[i][j] = userPuzzle[i][j];
        }
    }

    if(!solvePuzzle(tempPuzzle)){
        printf("\nincorrect value for the X = %d Y = %d coordinate, please try again\n",positionX + 1, positionY + 1);
        userPuzzle[positionY][positionX] = 0;
    }
    getchar();
    printPuzzle(userPuzzle);

    return;
}
