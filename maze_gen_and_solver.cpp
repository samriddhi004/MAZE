#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <vector>

const int screen_width = 400;
const int screen_height = 400;

const int rows = 10;
const int cols = 10;

bool m_previous = false; // keypress state FOR M aze generation
bool s_previous = false; // keypress state for S olving maze

enum states {
    PREMAZE,
    GENMAZE,
    PRESOLVE,
    SOLVING,
    SOLVED
};

int state = PREMAZE;

class Spot : public sf::Drawable
{
    private:
    sf::Color col = sf::Color::White;
    public:
    bool visited = false;
    float f = 0;  // f1-score for A* search algorithm
    float g = 0;  // cost to move from initial cell to current cell
    float h = 0;  // heuristic; an estimate of cost from current cell to endcell, here the euclidean distance

    float i = 0;    // x-axis
    float j = 0;    // y-axis

    std::vector<bool> walls = {true,true,true,true};  // top,right,bottom,down
    std::vector<Spot*> neighbors;
    Spot* previous = 0;

    void addNeighbors(std::vector<std::vector<Spot>> &cells){ // for maze generation
        neighbors.clear();
        if(i < cols - 1){  // i isn't at the rightmost end
            if(!cells[i+1][j].visited)
            neighbors.push_back(&cells[i+1][j]);
        }
        if(i > 0){  // i isnt at the leftmost end
            if(!cells[i-1][j].visited)
            neighbors.push_back(&cells[i-1][j]);
        }
        if(j < rows-1){  // j isnt at the very bottom
            if(!cells[i][j+1].visited)
            neighbors.push_back(&cells[i][j+1]);
        }
        if(j>0){ // j isnt at the very top
            if(!cells[i][j-1].visited)
                neighbors.push_back(&cells[i][j-1]);
        }
    }

    void addMazeNeighbors(std::vector<std::vector<Spot>> &cells){ // for solving maze
        neighbors.clear();
        //walls => {top,right,bottom,left}
        if(i<cols -1 && !walls[1]){ // if not at last column and no walls to the right
            neighbors.push_back(&cells[i+1][j]); // the right neighbor is pushed
        }
        if(i>0 && !walls[3]){ // not the first column and no wall to the left 
            neighbors.push_back(&cells[i-1][j]);
        }
        if(j<rows-1 && !walls[2]){ // not bottom most row and no wall directly below
            neighbors.push_back(&cells[i][j+1]);
        }
        if(j>0 && !walls[0]){ //not first row and exists a wall up above
            neighbors.push_back(&cells[i][j-1]);
        }
    }
    void setij(int x, int y){
        i = x;
        j = y;
    }
    void setCol(sf::Color new_col){
        col = new_col;
    }
    virtual void draw(sf::RenderTarget& target,sf::RenderStates states) const{
        
        int cell_w  = screen_width/cols;
        int cell_h = screen_height/rows;
        sf::RectangleShape shape(sf::Vector2f(cell_w,cell_h));
        shape.move(sf::Vector2f(i*cell_w,j*cell_h));
        shape.setFillColor(col);
        target.draw(shape);

        if(walls[0]){
            sf::VertexArray wall(sf::Lines,2);
            wall[0].position = sf::Vector2f(i*cell_w,j*cell_h); //(x,y)
            wall[0].color = sf::Color::Black;
            wall[1].position = sf::Vector2f((i+1)*cell_w,(j*cell_h));
            wall[1].color = sf::Color::Black;
            target.draw(wall);
        } //topwall

        if(walls[1]){ // right i+1,j
            sf::VertexArray wall(sf::Lines,2);
            wall[0].position = sf::Vector2f((i+1)*cell_w,j*cell_h);
            wall[0].color = sf::Color::Black;
            wall[1].position = sf::Vector2f((i+1)*cell_w,(j+1)*cell_h);
            wall[1].color = sf::Color::Black;
            target.draw(wall);
        }

        if(walls[2]){ // bottom i,j+1
            sf::VertexArray wall(sf::Lines,2);
            wall[0].position = sf::Vector2f((i)*cell_w,(j+1)*cell_h);
            wall[0].color = sf::Color::Black;
            wall[1].position = sf::Vector2f((i+1)*cell_w,(j+1)*cell_h);
            wall[1].color = sf::Color::Black;
            target.draw(wall);
        }

        if(walls[3]){ // left i,j
            sf::VertexArray wall(sf::Lines,2);
            wall[0].position = sf::Vector2f((i)*cell_w,(j*cell_h));
            wall[0].color = sf::Color::Black;
            wall[1].position = sf::Vector2f((i)*cell_w,(j+1)*cell_h);
            wall[1].color = sf::Color::Black;
            target.draw(wall);
        }

    }
};

std::vector<Spot> tmp(rows); //creates 1d vector called tmp that stores "rows" number of Spot objects
std::vector<std::vector<Spot>> grid(cols,tmp); 

// tmp = [ Spot, Spot, Spot, ... ]  // One row with 'rows' number of Spot objects

// grid = [
//     [ Spot, Spot, Spot, ... ],   // Column 1 (initialized with a copy of tmp)
//     [ Spot, Spot, Spot, ... ],   // Column 2 (initialized with a copy of tmp)
//     [ Spot, Spot, Spot, ... ],   // Column 3 (initialized with a copy of tmp)
//     ...
// ]  // 2D grid with 'cols' number of columns, and each column represents a copy of tmp
//GET BACK HERE AGAIN

std::vector<Spot*> openSet;
std::vector<Spot*> closedSet;
std::vector<Spot*>cell_stack;

Spot* start = &grid[0][0];
Spot* end = &grid[cols-1][rows-1];

void addtoOpenSet(Spot* cell){ //tracks cell that are open to explore in A*
    openSet.push_back(cell);
    cell->setCol(sf::Color(180,255,180)); // GREEEN 
}
void addtoClosedSet(Spot* cell){ // tracks cell that are unavailable to explore in A* cause it's the final path,DUHHH!
    closedSet.push_back(cell);
    cell->setCol(sf::Color::Yellow);
}
void removeFromArray(Spot* cell, std::vector<Spot*> &vect){
    for(int i = 0; i < (int)vect.size();i++){
        if(vect[i]==cell){
            vect.erase(vect.begin()+i); 
        }
    }
}
bool arrayHasElem(Spot* cell,std::vector<Spot*> &vect){
    for(int i = 0; i<(int)vect.size();i++){
        if(vect[i]==cell){
            return true;
        }
    }
    return false;
}

void removeWall(Spot* a, Spot* b) {
    int x = a->i - b->i;
    int y = a->j - b->j;
    if (x == 1) {   //  b-->a
        a->walls[3] = false;
        b->walls[1] = false;
    }
    else if (x == -1) { // a-->b
        a->walls[1] = false;
        b->walls[3] = false;
    }
    else if (y == 1) {                  // b (up)
        a->walls[0] = false;            // a (down) b to a gako downwards
        b->walls[2] = false;
    }
    else if (y == -1) {                 // a    (height vanda ni depth is considered here and as positive)
        a->walls[2] = false;            // b   (b bata a gako upwards)
        b->walls[0] = false;
    }
}

bool checkWall(Spot* a, Spot* b){
    int x = a->i - b->i ;
    int y = a->j - b->j;
    if(x==1){
        return a->walls[3];
    }
    if(x==-1){
        return a->walls[1];
    }
    if(y==1){
        return a->walls[0];
    }
    if(y==-1){
        return a->walls[2];
    }
    return 0;
}

float heuristic(Spot* a, Spot* b){ //euclidean distance as heuristic
    return std::sqrt((a->i-b->i)*(a->i-b->i) + (a->j - b->j)*(a->j - b->j));
}

void checkInput(){
    bool m_current = sf::Keyboard::isKeyPressed(sf::Keyboard::M);
    bool s_current = sf::Keyboard::isKeyPressed(sf::Keyboard::S);

    if(!m_previous && m_current){ //M pressed so generate maze
        if(state == PREMAZE){
            state = GENMAZE;
        }
    }

    if(!s_previous && s_current){ //S pressed to solve Maze
        if(state = PRESOLVE){
            addtoOpenSet(start);
            state = SOLVING;
        }
    }
}

// void resetMaze() {
//     openSet.clear();
//     closedSet.clear();
//     cell_stack.clear();

//     // Reset visited states of cells
//     for (int i = 0; i < cols; i++) {
//         for (int j = 0; j < rows; j++) {
//             grid[i][j].visited = false;
//         }
//     }
//     start = &grid[0][0];
//     end = &grid[cols - 1][rows - 1];
//     state = PREMAZE;
// }

int main() {
    sf::RenderWindow window(sf::VideoMode(screen_width, screen_height), "MAZE");
    // window is a class of RenderWindow and its basically a constructor initialized with height, width of screen in certain mode and the title of the screen
    std::srand(static_cast<unsigned>(std::time(nullptr)));
     
    for(int i =0;i<cols;i++){
        for(int j = 0; j<rows;j++){
            grid[i][j].setij(i,j);
        }
    }
    Spot* current = &grid[0][0];
    current->visited = true;
    Spot* next = nullptr;
    while (window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed)
                window.close();
        }
    window.clear();
    checkInput();

    if(state == GENMAZE){
        current->addNeighbors(grid);
        if(current->neighbors.size()>0){
            float r = static_cast<float> (std::rand())/ static_cast<float>(RAND_MAX); // between 0 and 1
            int i = std::floor(r * current->neighbors.size()); // in the range of neigbors size
            next = current->neighbors[i];
            current->neighbors.erase(current->neighbors.begin()+i);
        } 
        if(next!=nullptr){
            next->visited = true;
            cell_stack.push_back(current);
            removeWall(current,next);
            current = next;
            next = nullptr;
            // sf::sleep(sf::milliseconds(50)); 
        }
        else{
            if(cell_stack.size()==0){
                state = PRESOLVE;
                for(int i =0;i<cols;i++){
                    for(int j =0;j<rows;j++){
                        grid[i][j].addMazeNeighbors(grid);
                    }
                }
            } else{
                current = cell_stack[cell_stack.size()-1];  //peeking top of stack
                cell_stack.pop_back();
            }
        }
    }


    
    if(state == SOLVING){
        if(openSet.size()>0){ // we have unvisited neighbors that need to be explored
             int winner = 0; //index with lowest f score among the neighbors
                for (int i = 0; i < (int)openSet.size(); i++) {
                    if (openSet[i]->f < openSet[winner]->f) {
                        winner = i;
                    }
                }
                
                current = openSet[winner];
                
                if (openSet[winner] == end) {
                    std::cout << "Done!" << std::endl;
                    state = SOLVED;
                }
                
                removeFromArray(current, openSet);
                addtoClosedSet(current);
                
                for (int i = 0; i < (int)current->neighbors.size(); i++) {          // iterate through all neighbors of current
                    if (!arrayHasElem(current->neighbors[i], closedSet) && !checkWall(current, current->neighbors[i])/* && !current->neighbors[i]->wall */) {     // if current neighbor is not in the closed set and neighbor is not a wall
                        float tempG = current->g + 1;
                        
                        if (arrayHasElem(current->neighbors[i], openSet)) {    // if current neighbor is in the open set (already has a G value)
                            if (tempG < current->neighbors[i]->g) {
                                current->neighbors[i]->g = tempG;                   // if tentative G score is better than current, set new G value
                                current->neighbors[i]->previous = current;
                            }
                        } else {
                            current->neighbors[i]->g = tempG;                       // if current neighbor is not in open set, add to open set and set new G value
                            addtoOpenSet(current->neighbors[i]);
                            current->neighbors[i]->h = heuristic(current->neighbors[i], end);
                            current->neighbors[i]->previous = current;
                        }
                        current->neighbors[i]->f = current->neighbors[i]->g + current->neighbors[i]->h;
                    }
                }
                
            } else {
                // no solution
                std::cout << "no solution" << std::endl;
                state = SOLVED;
            }
            // coloring explored cells(u don't know if this is the best path )
            for (int i = 0; i < (int)closedSet.size(); i++) {
                closedSet[i]->setCol(sf::Color(255,180,180)); //explored paths only LIGHTPINK 
                // sf::sleep(sf::milliseconds(5)); 
            }
        }
        // the good and optimal path, by backtracking and coloring 
        if (state == SOLVING || state == SOLVED) {
            while (current->previous != 0) {
                current->setCol(sf::Color(180,180,255)); //BLUE 
                current = current->previous;
            // sf::sleep(sf::milliseconds(50)); 
            }
            current->setCol(sf::Color(180,180,255)); //BLUE
            // sf::sleep(sf::milliseconds(5));
        }
        // if(state == SOLVED){
        //     if(sf::Keyboard::isKeyPressed(sf::Keyboard::M)){
        //         resetMaze();
        //     }
        // }
        
        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                if (state == GENMAZE) {
                    if (grid[i][j].visited)
                        //grid[i][j].setCol(sf::Color::Magenta);
                        grid[i][j].setCol(sf::Color(255,180,255));
                    current->setCol(sf::Color::Blue);
                } else if (state == PRESOLVE) {
                    grid[i][j].setCol(sf::Color::White);
                }
                window.draw(grid[i][j]);

            }
        }
        
        
        window.display();
    }

    return 0;
}
