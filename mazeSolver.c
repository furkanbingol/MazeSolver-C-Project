//<Furkan Bingöl>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

enum states{
    wall,      
    empty,     
    apple,     
    target,    
    path      
};

char **getMaze(char *fileName, int *rows, int *cols, int *startRow, int *startCol, char **maze);
int  **getVisited(int *rows, int *cols, int **visited, char **maze);
void addApples(int rows, int cols, int **visited, char **maze);
void printMaze(int *rows, int *cols, char **maze);
int mazeSolver_dfs(int row, int col, int *rows, int *cols, int *totalScore, int **visited, char **maze);
int isDeadEnd(int row, int col, char **maze);
void freeArrays(int rows, char **maze, int **visited, char **pathMaze, int **pathVisited); 
void printMainPath(int rows, int cols, char **maze, char **pathMaze);
char **maze_copy(int rows, int cols, char **maze, char **pathMaze);
int **visited_copy(int rows, int cols, int **visited, int **pathVisited);
int mainPath(int row, int col, int **pathVisited);
void addPath(int rows, int cols, char **pathMaze, int **pathVisited);
void sleepTime();
void SetConsoleColor(WORD* Attributes, DWORD Colour);   
void ResetConsoleColor(WORD Attributes); 


int main(){
    srand(time(NULL));
    char **maze,**pathMaze;
    int **visited,**pathVisited;   
    int rows,cols;
    int startRow,startCol;     
    int totalScore = 0;

    maze = getMaze("maze.txt",&rows,&cols,&startRow,&startCol,maze);
    visited = getVisited(&rows,&cols,visited,maze);
    addApples(rows,cols,visited,maze);
    pathMaze = maze_copy(rows,cols,maze,pathMaze);
    pathVisited = visited_copy(rows,cols,visited,pathVisited);

    printf("Total Score: %d\n\n",totalScore);
    printMaze(&rows,&cols,maze);
    mazeSolver_dfs(startRow,startCol,&rows,&cols,&totalScore,visited,maze);
    printf("\n\n");

    mainPath(startRow,startCol,pathVisited);
    addPath(rows,cols,pathMaze,pathVisited);
    printMainPath(rows,cols,maze,pathMaze);
    printf("TOTAL SCORE: %d\n",totalScore);
    freeArrays(rows,maze,visited,pathMaze,pathVisited);
    return 0;
}

char **getMaze(char *fileName, int *rows, int *cols, int *startRow, int *startCol, char **maze){
    int i,j;
    int r,c;
    char ch;
    FILE *fp = fopen(fileName,"r");
    
    if(fp != NULL)
        fscanf(fp, "%d %d", &r, &c);
    *rows = r;
    *cols = c;
    ch = getc(fp);


    //allocation
    maze = (char**)malloc(sizeof(char*) * (*rows));

    for(i = 0; i < *rows; i++){
        maze[i] = (char*)malloc(sizeof(char) * (*cols));
    }

    //matrix reading    
    for(i = 0; i < *rows; i++){
        for(j = 0; j < *cols; j++){
            ch = getc(fp);
            if(ch == '\n')
                ch = getc(fp);
            
            maze[i][j] = ch;

            if(ch == 'b'){
                *startRow = i;
                *startCol = j;
            }
        }
    }
    fclose(fp);
    return maze;
}

int **getVisited(int *rows, int *cols, int **visited, char **maze){
    int i,j;
    
    //allocation
    visited = (int**)malloc(sizeof(int*) * (*rows));

    for(i = 0; i < *rows; i++){
        visited[i] = (int*)malloc(sizeof(int) * (*cols));
    }


    for(i = 0; i < *rows; i++){
        for(j = 0; j < *cols; j++){
            if(maze[i][j] == '+' || maze[i][j] == '-' || maze[i][j] == '|')
                visited[i][j] = wall;
            else if(maze[i][j] == 'c')
                visited[i][j] = target;
            else
                visited[i][j] = empty;
        }
    }
    return visited;
}

void addApples(int rows, int cols, int **visited, char **maze){
    int i,j;
    int numberOfApples = 15;    
    
    printf("NumberOfApples: %d\n",numberOfApples);

    while(numberOfApples > 0){
        do{
            i = rand() % (rows-2) + 1;
            j = rand() % (cols-2) + 1;
        } while( (visited[i][j] != empty) || (maze[i][j] == 'b') );

        visited[i][j] = apple;
        maze[i][j] = 'O';
        numberOfApples--;
    }
    sleepTime();
}

int mazeSolver_dfs(int row, int col, int *rows, int *cols, int *totalScore, int **visited, char **maze){
    int *point = &visited[row][col];

    if(*point == target)
        return 1;

    if(*point == empty || *point == apple){
        *point = path;

        if(maze[row][col] == 'O'){     
            *totalScore += 10;         
            maze[row][col] = '&';      
        }
        else if(maze[row][col] == ' '){
            maze[row][col] = '*';
        }

        if(isDeadEnd(row,col,maze) == 1)    
            *totalScore -= 5; 

        sleepTime(); //~0.5s
        printMaze(rows, cols, maze);       
        printf("Total Score: %d\n",*totalScore);
        
        if(mazeSolver_dfs(row, col+1, rows, cols, totalScore, visited, maze) == 1)     //checking right
            return 1;

        if(mazeSolver_dfs(row, col-1, rows, cols, totalScore, visited, maze) == 1)     //checking left
            return 1;

        if(mazeSolver_dfs(row-1, col, rows, cols, totalScore, visited, maze) == 1)     //checking up
            return 1;

        if(mazeSolver_dfs(row+1, col, rows, cols, totalScore, visited, maze) == 1)     //checking down
            return 1;     
    }
    return 0;
}

int isDeadEnd(int row, int col, char **maze){   
    if(maze[row][col+1] != ' ' && maze[row][col-1] != ' ' && maze[row+1][col] != ' ' && maze[row-1][col] != ' '){
        if(maze[row][col+1] != 'O' && maze[row][col-1] != 'O' && maze[row+1][col] != 'O' && maze[row-1][col] != 'O'){
            if(maze[row][col+1] != 'c' && maze[row][col-1] != 'c' && maze[row+1][col] != 'c' && maze[row-1][col] != 'c')
                return 1;   
        }
    }
    else
        return 0;
}

void printMaze(int *rows, int *cols, char **maze){
    int i,j;
    WORD Attributes = 0;

    for(i = 0; i < *rows; i++){
        for(j = 0; j < *cols; j++){
            if(maze[i][j] == '&'){              
        		SetConsoleColor(&Attributes, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
                printf("%c ",maze[i][j]);
        		ResetConsoleColor(Attributes);
			}
            else if(maze[i][j] == '*'){         
                SetConsoleColor(&Attributes, FOREGROUND_INTENSITY | FOREGROUND_BLUE);
        		printf("%c ",maze[i][j]);
                ResetConsoleColor(Attributes);
            }
            else if(maze[i][j] == 'O'){         
                SetConsoleColor(&Attributes, FOREGROUND_INTENSITY | FOREGROUND_RED);
        		printf("%c ",maze[i][j]);
                ResetConsoleColor(Attributes);
            }
            else
            	printf("%c ",maze[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

char **maze_copy(int rows, int cols, char **maze, char **pathMaze){
    int i,j;
    
    pathMaze = (char**)malloc(sizeof(char*) * rows);
    for(i = 0; i < rows; i++){
        pathMaze[i] = (char*)malloc(sizeof(char) * cols);
    }

    for(i = 0; i < rows; i++){
        for(j = 0; j < cols; j++){
            pathMaze[i][j] = maze[i][j];
        }
    }

    return pathMaze;
}

int **visited_copy(int rows, int cols, int **visited, int **pathVisited){
    int i,j;

    pathVisited = (int**)malloc(sizeof(int*) * rows);
    for(i = 0; i < rows; i++){
        pathVisited[i] = (int*)malloc(sizeof(int) * cols);
    }

    for(i = 0; i < rows; i++){
        for(j = 0; j < cols; j++){
            pathVisited[i][j]= visited[i][j]; 
        }
    }

    return pathVisited;
}

int mainPath(int row, int col, int **pathVisited){
    int *point = &pathVisited[row][col];

    if(*point == target)
        return 1;

    if(*point == empty || *point == apple){
        *point = wall;

        if(mainPath(row, col+1, pathVisited) == 1){
            *point = path;
            return 1;
        }
            
        if(mainPath(row, col-1, pathVisited) == 1){
            *point = path;
            return 1;
        } 

        if(mainPath(row-1, col, pathVisited) == 1){
            *point = path;
            return 1;
        }    

        if(mainPath(row+1, col, pathVisited) == 1){
            *point = path;
            return 1;
        }         
    }
    return 0;
}

void addPath(int rows, int cols, char **pathMaze, int **pathVisited){
    int i,j;
    for(i = 0; i < rows; i++){
        for(j = 0; j < cols; j++){
            if(pathMaze[i][j] != 'b' && pathMaze[i][j] != 'c'){
                if(pathVisited[i][j] == path)
                    pathMaze[i][j] = '*';
            }
        }
    }
}

void printMainPath(int rows, int cols, char **maze, char **pathMaze){
    int i,j;
    WORD Attributes = 0;

    for(i = 0; i < rows; i++){
        for(j = 0; j < cols; j++){
            if(maze[i][j] == '&'){
        		SetConsoleColor(&Attributes, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
                printf("%c ",maze[i][j]);
        		ResetConsoleColor(Attributes);
			}
            else if(maze[i][j] == 'O'){
                SetConsoleColor(&Attributes, FOREGROUND_INTENSITY | FOREGROUND_RED);
        		printf("%c ",maze[i][j]);
                ResetConsoleColor(Attributes);
            }
            else if(pathMaze[i][j] == '*'){
                SetConsoleColor(&Attributes, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
        		printf("%c ",pathMaze[i][j]);
                ResetConsoleColor(Attributes);
            }
            else
                printf("%c ",pathMaze[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void freeArrays(int rows, char **maze, int **visited, char **pathMaze, int **pathVisited){
    int i;

    for (i = 0; i < rows; i++){  
       free(maze[i]);
       free(visited[i]);
       free(pathMaze[i]);
       free(pathVisited[i]);  
    }     
    free(maze);
    free(visited);
    free(pathMaze);
    free(pathVisited);
}

void sleepTime(){ //~0.5s
    int i,j,x;
    for(i = 0; i < 2000000; i++){
        for(j = 0; j < 240; j++){
            x = 0;
        }
    }
}

void SetConsoleColor(WORD* Attributes, DWORD Colour)
{
    CONSOLE_SCREEN_BUFFER_INFO Info;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hStdout, &Info);
    *Attributes = Info.wAttributes;
    SetConsoleTextAttribute(hStdout, Colour);
}

void ResetConsoleColor(WORD Attributes)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Attributes);
}
