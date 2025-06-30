#ifndef SUDOKU_H
#define SUDOKU_H

#include <algorithm>
#include <ctime>
#include <iostream>
#include <stack>
#include <vector>

#include <chrono>
using namespace std;
class Sudoku {

public:
    Sudoku();
    Sudoku(vector<vector<char>>& board);

    int solution();
    int backtrack();
    void print_candidate(int flag);
    void print_sudoku();

    bool check_unique_row(char i, char j, char numb);
    bool check_unique_col(char i, char j, char numb);
    bool check_unique_pal(char i, char j, char numb);

    int solver_with_no_backtrack();

    bool delete_row(char i, char j, char numb);
    bool delete_col(char i, char j, char numb);
    bool delete_pal(char i, char j, char numb);
    bool delete_self(char i, char j);
    int _main_();

private:
    vector<vector<char>> sudoku;
    vector<vector<vector<bool>>> candidate; // candidate[9][9].back() = 1/2/3/4/5/6/7/8/9

    void init_candidate();
};

#endif // SUDOKU_H
