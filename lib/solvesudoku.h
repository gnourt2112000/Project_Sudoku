char* createPuzzle();
void printPuzzle(char* puzzle);
bool checkAvailable(char* puzzle, int* row, int* column);
bool checkBox(char* puzzle, int row, int column, int val);
bool solvePuzzle(char* puzzle);
char* copyPuzzle(char* puzzle);
void userChoice(char* userPuzzle, char* tempPuzzle,int positionY, int positionX, int userVal);
