#include <iostream>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <chrono>
#include <thread>

// using namespace std;
const int rows = 21; 
const int cols = 21;
int maze[rows][cols];

void swap(int& a, int& b){
    int temp;
    temp = a;
    a = b;
    b = temp;
}

enum Direction{
    U,D,L,R
};

bool isValid(int row, int col){
    return row>=0 && row<rows && col >=0 && col<cols && maze[row][col] ==0;
}

void initMaze(){
    srand (static_cast<unsigned>(time(0)));
    for(int i =0;i<rows;i++){
        for(int j = 0; j<cols; j++){
            maze[i][j] = 0; //all walls rn
        }
    }
}
void displayMaze(){
    for(int i =0; i<rows;i++){
        for(int j = 0; j<cols;j++){
            if(maze[i][j] == 0){
                std::cout<<" #";
            }
            else{
                std::cout<<" .";
            }
        }
        std::cout<<std::endl;
    }
}

void generateMaze(int row, int col){
    static int directions[] = {U,D,L,R};
    for (int i =0;i<4;++i){
        int randomIndex = i + rand() % (4-i);
        swap(directions[i],directions[randomIndex]);
    }
    for(int i = 0;i<4;i++){
        int newRow = row;
        int newCol = col;
        switch (directions[i])
        {
        case U:
            newRow -=2;
            break;
        case D:
            newRow +=2;
            break;
        case L:
            newCol -=2;
            break;
        case R:
            newCol +=2;
            break;
        }
        if(isValid(newRow,newCol)){
            maze[newRow][newCol] =1; // this is the path
            maze[row + (newRow-row)/2][col + (newCol - col)/2] = 1;
            // displayMaze();
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust the delay time
            generateMaze(newRow,newCol);
        }
    }
}

int main() {
    srand(static_cast<unsigned>(time_t(0)));
    initMaze();
    generateMaze(0,5);
    displayMaze();
    return 0;
}