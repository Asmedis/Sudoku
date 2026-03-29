#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <chrono>

using namespace std;

//---Structure to store sudoku table---//
struct Sudoku
{
    int size;
    int** table;
};

//---Total guess counter---//
long int steps = 0;

//---Time counter---//
auto start = chrono::high_resolution_clock::now();
auto endt = chrono::high_resolution_clock::now();

Sudoku solveSudoku(Sudoku sudoku, bool show);

//---Function to read sudoku from file---//
Sudoku readSudoku(string file_name){
    ifstream File(file_name);
    Sudoku output;

    if (!File) {
        cerr << "Error: Unable to open file " << file_name << endl;
        exit(1);
    }
    File >> output.size;

    output.table = new int*[output.size];

    for (int x = 0; x < output.size; x++)
    {
        output.table[x] = new int[output.size];
        for(int y = 0; y < output.size; y++)
        {
            File>>output.table[x][y];
        }
    }
    File.close();
    return output;
}

//---Function to print sudoku table---//
void printSudoku(Sudoku sudoku) {
    int boxSize = sqrt(sudoku.size);
    int gridSize = sudoku.size;

    cout<<endl;

    int line_len = (gridSize * 2 + boxSize);

    if (boxSize> 3)  line_len = (gridSize * 3 + boxSize + 1);

    for (int x = 0; x < gridSize; x++) {
        if (x % boxSize == 0 && x != 0)
        {
            for (int i = 0; i < line_len; i++) cout << "-";
            cout << endl;
        }

        for (int y = 0; y < gridSize; y++) {
            if (y % boxSize == 0 && y != 0) cout << "| ";
            if (sudoku.table[x][y] == 0){
                if(boxSize > 3){
                    cout << "   ";
                }
                else{
                    cout << "  ";
                }
            }
            else{
                if(boxSize > 3){
                    cout << setw(2) << sudoku.table[x][y] << " ";
                }
                else{
                    cout << sudoku.table[x][y] << " ";
                }
            } 
        }
        cout << endl;
    }
}

//---Function that return possible numbers for a cell---//
vector<int> gridPossible(Sudoku sudoku, int x, int y)
{
    int gridSize = sqrt(sudoku.size);
    vector<int> listPossible;

    if(sudoku.table[x][y]!=0) return listPossible;

    for(int x = 0; x < sudoku.size; x++)
    {
        listPossible.push_back(x+1);
    }

    //Check rows and collums
    for(int i = 0; i < sudoku.size; i++){
        listPossible.erase(remove(listPossible.begin(), listPossible.end(), sudoku.table[i][y]), listPossible.end());
        listPossible.erase(remove(listPossible.begin(), listPossible.end(), sudoku.table[x][i]), listPossible.end());
    }
    int gridX = x - (x % gridSize);
    int gridY = y - (y % gridSize);

    //Check 3x3 grids.
    for(int i = 0; i < gridSize; i++)
    {
        for(int z = 0; z < gridSize; z++)
        {
            if(gridX + i != x && gridY + z != y)
            {
                listPossible.erase(remove(listPossible.begin(), listPossible.end(), sudoku.table[gridX+i][gridY+z]), listPossible.end());
            }
        }
    }
    return listPossible;
}

//---Function that check if sudoku is completed---//
bool isCompleted(Sudoku sudoku){
    for(int x = 0; x < sudoku.size; x++)
        {
            for(int y = 0; y < sudoku.size; y++)
            {
                if(sudoku.table[x][y] == 0){
                    return false;
                }
            }
        }
    return true;
}

//---Function to free sudoku table---//
void freeSudoku(Sudoku sudoku){
    for (int x = 0; x < sudoku.size; x++)
    {
        free(sudoku.table[x]);
    }
    free(sudoku.table);
}

//---Function to deep copy sudoku table---//
Sudoku deepCopySudoku(Sudoku src) {
    Sudoku dest;
    dest.size = src.size;
    dest.table = new int*[dest.size];

    for (int x = 0; x < dest.size; x++)
    {
        dest.table[x] = new int[dest.size];
        for(int y = 0; y < dest.size; y++)
        {
            dest.table[x][y] = src.table[x][y];
        }
    }
    return dest;
}

//---Function to guess a number in the cell with least possibilities---//
Sudoku guessSudoku(Sudoku sudoku, bool show = false){
    vector<int> possibilities;
    int lowest = sudoku.size;
    int lowX, lowY;

    //--Find the cell with the least possibilities--//
    for(int x = 0; x < sudoku.size; x++)
    {
        for(int y = 0; y < sudoku.size; y++)
        {
            if(sudoku.table[x][y] == 0)
            {
                possibilities = gridPossible(sudoku, x, y);
                if(possibilities[0] == 0 || possibilities.size()==0)
                {                  
                    return sudoku;
                }
                if (possibilities.size() < lowest)
                {
                    lowest = possibilities.size();
                    lowX = x;
                    lowY = y;
                }
            }
        }
    }

    Sudoku temp;
    possibilities = gridPossible(sudoku, lowX, lowY);
    if(possibilities[0] == 0 || possibilities.size() < 2){
        return sudoku;
    }

    temp = deepCopySudoku(sudoku);
    //--Try all possibilities in the cell with least possibilities--//
    for(int i = 0; i < possibilities.size(); i++){
        sudoku.table[lowX][lowY] = possibilities[i];
        sudoku = solveSudoku(sudoku, show);
        if(!isCompleted(sudoku)){
            //--If sudoku is not completed, try next possibility--//
            freeSudoku(sudoku);
            sudoku = deepCopySudoku(temp);
        }
    }
    return sudoku;
}

Sudoku solveSudoku(Sudoku sudoku, bool show = false){
    steps++;
    //--Show sudoku every 100000 steps (optimization)--//
    if(show == 1 && steps%100000==0)
    {
        system("cls");
        printSudoku(sudoku);
        cout<<steps/100000<<"k"<<endl;
        endt = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = endt - start;
        cout<<"Time: "<< elapsed.count() <<"s./1k"<<endl;
        start = chrono::high_resolution_clock::now();
    }
    //--Fill in all gaps with one possibility--//
    bool found = false;
    vector<int> possibilities;
    for(int x = 0; x < sudoku.size; x++)
    {
        for(int y = 0; y < sudoku.size; y++)
        {
            if(sudoku.table[x][y] == 0){

                possibilities = gridPossible(sudoku, x, y);
                if(possibilities[0] == 0){
                    return sudoku;
                }
                if(possibilities.size() == 1)
                {
                    sudoku.table[x][y] = possibilities[0];
                    found = true;
                    sudoku = solveSudoku(sudoku, show);
                    break;
                }
            }
        }
    }
    //--Return if Sudoku is completed--//
    if(!isCompleted(sudoku)){
        sudoku = guessSudoku(sudoku, show);
    }
    return sudoku;
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    char buf[1 << 20], *p1 = buf, *p2 = buf;
    #define getchar() (p1 == p2 && (p2 = (p1 = buf) + fread(buf, 1, 1 << 20, stdin), p1 == p2) ? EOF : *p1++)

    Sudoku sudoku = readSudoku("Sudoku_files/test4.txt");

    printSudoku(sudoku);

    auto start_final = chrono::high_resolution_clock::now();
    sudoku = solveSudoku(sudoku);
    auto end_final = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end_final - start_final;

    printSudoku(sudoku);
    cout<<"Steps: "<<steps<<endl;

    cout << "Execution time: " << elapsed.count() << " seconds\n";
    freeSudoku(sudoku);
}

//.avl medziai

