#include "sudoku.h"
vector<vector<char>> test = {
    { 4, 0, 0, 0, 0, 0, 0, 0, 9 },
    { 0, 2, 0, 7, 0, 3, 0, 4, 0 },
    { 0, 0, 6, 0, 0, 0, 1, 0, 0 },
    { 0, 9, 0, 1, 0, 4, 0, 7, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 8, 0, 2, 0, 6, 0, 5, 0 },
    { 0, 0, 3, 0, 0, 0, 6, 0, 0 },
    { 0, 1, 0, 5, 0, 9, 0, 8, 0 },
    { 7, 0, 0, 0, 0, 0, 0, 0, 2 }
};
Sudoku::Sudoku()
{
    //    vector<vector<char>> cv_suduko = {
    //        { 0, 0, 0, 6, 0, 4, 7, 0, 0 },
    //        { 7, 0, 6, 0, 0, 0, 0, 0, 9 },
    //        { 0, 0, 0, 0, 0, 5, 0, 8, 0 },
    //        { 0, 7, 0, 0, 2, 0, 0, 9, 3 },
    //        { 8, 0, 0, 0, 0, 0, 0, 0, 5 },
    //        { 4, 3, 0, 0, 1, 0, 0, 7, 0 },
    //        { 0, 5, 0, 2, 0, 0, 0, 0, 0 },
    //        { 3, 0, 0, 0, 0, 0, 2, 0, 8 },
    //        { 0, 0, 2, 3, 0, 1, 0, 0, 0 }
    //    };
    vector<vector<char>> cv_suduko = test;
    sudoku.resize(9, vector<char>(9, '.'));
    candidate.resize(10, vector<vector<bool>>(10, vector<bool>(10, true)));

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (cv_suduko[i][j] >= 0 && cv_suduko[i][j] <= 9) {

                sudoku[i][j] = cv_suduko[i][j];
            }
        }
    }

    init_candidate();
}

Sudoku::Sudoku(vector<vector<char>>& board)
{
    sudoku.resize(9, vector<char>(9, '.'));
    candidate.resize(10, vector<vector<bool>>(10, vector<bool>(10, true)));

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (board[i][j] >= 0 && board[i][j] <= 9) {

                sudoku[i][j] = board[i][j];
            }
        }
    }
    print_sudoku();

    init_candidate();
    // print_candidate( 0);
    int ret = solver_with_no_backtrack();
    print_candidate(1);
    if (ret == 0)
        cout << "已解决" << endl;
    // print_candidate( 5);
    print_sudoku();
}

int Sudoku::solution()
{

    int ret = backtrack();

    if (ret == 0) {

        cout << "success" << endl;

    } else if (ret == 1)
        cout << "failure" << endl;

    print_sudoku();
    return 0;
}

/*
int 回溯(候选表，数独表){
        检查当前状态check(数独表,候选表) 找出空白位置且候选最少的那个格子 Grid
                Grid不存在 => 完成
                        return 1

                Grid的候选数量==0  =>违规
                        retern -1

                Grid的候选数量>=1:
                        for(候选)
                                尝试填入
                                删除候选（无论该候选数是否正确，都不会再第二次使用）
                                ret = 回溯()
                                if(ret==-1)
                                        擦除填入
                                else if(ret ==1)
                                        return 1
                        如果进行到这一步，意味着候选找完了，也没能确定一个值
                        => return -1
} */
int Sudoku::backtrack()
{

    int ret = solver_with_no_backtrack();

    if (ret == 0) // 已解出
        return 0;
    else if (ret == -1) // 冲突
        return -1;
    // else if(ret == 1)
    // 需要回溯
    //
    //
    // 找一个坐标i,j
    int findout_i = 0;
    int findout_j = 0;
    int candi_min_count = 10;
    for (int find_i = 0; find_i < 9; find_i++) {
        for (int find_j = 0; find_j < 9; find_j++) {
            if (sudoku[find_i][find_j] != 0)
                continue;
            int count = 0;
            // 一个格子的所有候选
            for (int numb = 1; numb <= 9; numb++) {
                if (candidate[find_i][find_j][numb] == true)
                    count++;
            }
            if (candi_min_count > count) {
                candi_min_count = count;
                findout_i = find_i;
                findout_j = find_j;
            }
        }
    }
    for (int cnt = 1; cnt <= 9; cnt++) {
        if (candidate[findout_i][findout_j][cnt] == false)
            continue;

        Sudoku branch_copy = *this; // 深拷贝当前状态
        // 修改(branch_copy.sudoku, branch_copy.candidate);
        branch_copy.sudoku[findout_i][findout_j] = cnt;
        branch_copy.delete_row(findout_i, findout_j, cnt);
        branch_copy.delete_col(findout_i, findout_j, cnt);
        branch_copy.delete_pal(findout_i, findout_j, cnt);
        branch_copy.delete_self(findout_i, findout_j);

        // printf("【%d个候选，回溯尝试填入(%d,%d):%d】\n", candi_min_count, findout_i, findout_j, cnt);

        ret = branch_copy.backtrack();

        if (ret == 0) {
            *this = branch_copy;
            return 0;
        }
    }

    if (ret == -1) {
        return -1;
    }
}

void Sudoku::print_candidate(int flag)
{
    return;

    for (int i = 0; i < 9; i++) {
        // if (sudoku[0][i] == 0)
        // continue;
        printf("%d:", sudoku[flag][i]);
        for (int n = 1; n <= 9; n++) {
            if (candidate[flag][i][n] == true)
                cout << n;
            else
                cout << ".";
        }
        cout << endl;
    }
}

void Sudoku::print_sudoku()
{
    cout << "*******Sudoku*********" << endl;
    int i = 1;
    for (vector<char>& line : sudoku) {

        for (char& c : line) {
            printf("%d ", c);
        }
        cout << endl;

        if (i == 3 || i == 6)
            cout << "-------------------" << endl;
        i++;
    }
    cout << "*********************" << endl;
}

// 是否横向唯一
bool Sudoku::check_unique_row(char i, char j, char numb)
{
    for (int heng = 0; heng < 9; heng++) {
        if (heng != j && candidate[i][heng][numb] == true)
            return false;
    }

    return true;
}

bool Sudoku::check_unique_col(char i, char j, char numb)
{

    for (int shu = 0; shu < 9; shu++) {
        if (shu != j && candidate[shu][j][numb] == true)
            return false;
    }

    return true;
}

bool Sudoku::check_unique_pal(char i, char j, char numb)
{

    char start_i = 3 * (i / 3);
    char start_j = 3 * (j / 3);
    // 该宫的起点是(start_i,start_j)
    // printf("(%d,%d) ", start_i, start_j);
    for (int index_i = start_i; index_i < start_i + 3; index_i++) {
        for (int index_j = start_j; index_j < start_j + 3; index_j++) {
            if (index_i == i || index_j == j && candidate[index_i][index_j][numb] == true)
                return false;
        }
    }

    // cout << "宫唯一:" << int(numb) << "; ";
    return true;
}
int Sudoku::solver_with_no_backtrack()
{
    int x = -1;
    int y = -1;
    int numb = 0;
    int min_count = 10;
    ;
    bool operate = true;
    int no_empty_count = 0; // 记录一轮下来，非空的格子数
    while (operate) {

        operate = false;
        no_empty_count = 0;
        for (int shu = 0; shu < 9; shu++) {
            for (int heng = 0; heng < 9; heng++) {

                if (sudoku[shu][heng] != 0) // 如果是非空的格子，跳过
                {
                    no_empty_count++;
                    continue;
                }

                int true_count = 0;
                int numb_ = 0;

                // 得先遍历一次看是否违规
                for (int n_bool = 1; n_bool <= 9; n_bool++) {
                    if (candidate[shu][heng][n_bool] == true) {
                        true_count++;
                        numb_ = n_bool;
                    }
                }

                // 违规, 空格且无候选
                if (true_count == 0) {
                    return -1;
                }

                // 显式唯一候选
                if (true_count == 1) {

                    sudoku[shu][heng] = numb_;
                    delete_row(shu, heng, numb_);
                    delete_col(shu, heng, numb_);
                    delete_pal(shu, heng, numb_);
                    for (int nn = 1; nn <= 9; nn++)
                        candidate[shu][heng][nn] = false;

                    operate = true;
                    continue;
                }

                // 隐式唯一候选
                for (int n_bool = 0; n_bool < 9; n_bool++) {
                    if (candidate[shu][heng][n_bool] == true)
                        if (check_unique_row(shu, heng, n_bool) || check_unique_col(shu, heng, n_bool) || check_unique_pal(shu, heng, n_bool)) {

                            sudoku[shu][heng] = n_bool;

                            delete_row(shu, heng, n_bool);
                            delete_col(shu, heng, n_bool);
                            delete_pal(shu, heng, n_bool);
                            for (int nn = 1; nn <= 9; nn++)
                                candidate[shu][heng][nn] = false;
                            // 删去候选

                            operate = true;
                            continue;
                        }
                }
            }
        }
    }
    // cout << endl;

    // cout << "....solver_with_no_backtrack...." << endl;
    if (no_empty_count == 81)
        return 0;
    else
        return 1;
}

bool Sudoku::delete_row(char i, char j, char numb)
{

    for (int heng = 0; heng < 9; heng++) {
        candidate[i][heng][numb] = false;
    }
    return true;
}

bool Sudoku::delete_col(char i, char j, char numb)
{
    for (int shu = 0; shu < 9; shu++) {
        candidate[shu][j][numb] = false;
    }
    return true;
}

bool Sudoku::delete_pal(char i, char j, char numb)
{

    char start_i = 3 * (i / 3);
    char start_j = 3 * (j / 3);
    // 该宫的起点是(start_i,start_j)
    // printf("(%d,%d) ", start_i, start_j);
    for (int index_i = start_i; index_i < start_i + 3; index_i++) {
        for (int index_j = start_j; index_j < start_j + 3; index_j++) {
            candidate[index_i][index_j][numb] = false;
        }
    }

    return 0;

    return false;
}

bool Sudoku::delete_self(char i, char j)
{
    for (int nn = 1; nn <= 9; nn++)
        candidate[i][j][nn] = false;
    return false;
}

void Sudoku::init_candidate()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (sudoku[i][j] == 0)
                continue;

            delete_row(i, j, sudoku[i][j]);
            delete_col(i, j, sudoku[i][j]);
            delete_pal(i, j, sudoku[i][j]);
            delete_self(i, j);
        }
    }
}

int Sudoku::_main_()
{

    vector<vector<char>> cv_suduko = {
        { 0, 0, 0, 6, 0, 4, 7, 0, 0 },
        { 7, 0, 6, 0, 0, 0, 0, 0, 9 },
        { 0, 0, 0, 0, 0, 5, 0, 8, 0 },
        { 0, 7, 0, 0, 2, 0, 0, 9, 3 },
        { 8, 0, 0, 0, 0, 0, 0, 0, 5 },
        { 4, 3, 0, 0, 1, 0, 0, 7, 0 },
        { 0, 5, 0, 2, 0, 0, 0, 0, 0 },
        { 3, 0, 0, 0, 0, 0, 2, 0, 8 },
        { 0, 0, 2, 3, 0, 1, 0, 0, 0 }
    };

    vector<vector<char>> grid1 = {
        { 1, 0, 0, 0, 8, 0, 7, 2, 5 },
        { 7, 3, 8, 2, 6, 5, 0, 0, 1 },
        { 5, 4, 2, 0, 1, 7, 0, 0, 8 },
        { 0, 2, 4, 1, 7, 6, 5, 0, 3 },
        { 6, 0, 1, 5, 3, 9, 2, 0, 0 },
        { 0, 5, 0, 8, 4, 2, 0, 1, 6 },
        { 0, 0, 0, 7, 2, 0, 1, 5, 0 },
        { 2, 0, 5, 6, 0, 1, 8, 0, 0 },
        { 4, 1, 0, 0, 5, 8, 6, 0, 2 }
    };

    vector<vector<char>> grid2 = {
        { 0, 1, 0, 0, 9, 5, 0, 7, 6 },
        { 0, 9, 0, 7, 0, 0, 0, 0, 0 },
        { 7, 0, 0, 0, 0, 0, 4, 0, 0 },
        { 0, 6, 7, 0, 0, 4, 9, 0, 5 },
        { 5, 0, 0, 0, 6, 0, 0, 0, 0 },
        { 0, 0, 0, 5, 3, 0, 6, 8, 0 },
        { 0, 4, 9, 2, 0, 0, 5, 0, 0 },
        { 0, 0, 2, 8, 0, 3, 1, 9, 0 },
        { 0, 0, 5, 0, 1, 0, 0, 6, 0 }
    };

    vector<vector<char>> grid3 = {
        { 5, 0, 0, 0, 9, 5, 0, 7, 6 },
        { 0, 0, 4, 7, 0, 0, 0, 0, 0 },
        { 0, 6, 0, 0, 0, 0, 4, 0, 0 },
        { 0, 6, 7, 0, 0, 4, 9, 0, 5 },
        { 5, 0, 0, 0, 6, 0, 0, 0, 0 },
        { 0, 0, 0, 5, 3, 0, 6, 8, 0 },
        { 0, 4, 9, 2, 0, 0, 5, 0, 0 },
        { 0, 0, 2, 8, 0, 3, 1, 9, 0 },
        { 0, 0, 5, 0, 1, 0, 0, 6, 0 }
    };

    vector<vector<char>> superplus = {
        { 0, 7, 0, 0, 9, 0, 0, 0, 0 },
        { 0, 0, 1, 0, 0, 8, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 2, 9, 6 },
        { 0, 0, 0, 8, 0, 3, 0, 0, 0 },
        { 3, 0, 4, 2, 0, 0, 0, 1, 0 },
        { 0, 6, 0, 5, 0, 0, 0, 0, 0 },
        { 0, 0, 7, 0, 0, 0, 9, 0, 3 },
        { 0, 4, 0, 0, 6, 2, 0, 0, 0 },
        { 0, 9, 5, 0, 0, 0, 6, 0, 8 }
    };

    vector<vector<char>> tianma = {
        { 9, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 6, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 7, 0, 0, 0, 0, 0, 3, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 8, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0, 0, 6, 0, 5 }
    };

    vector<vector<char>> test = {
        { 0, 1, 0, 0, 0, 7, 0, 0, 0 },
        { 0, 2, 0, 0, 0, 3, 0, 0, 0 },
        { 0, 0, 3, 4, 0, 0, 9, 8, 0 },
        { 6, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 8, 5, 0, 0, 1, 0, 0, 3, 6 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 5 },
        { 0, 6, 7, 0, 0, 5, 3, 0, 0 },
        { 0, 0, 0, 8, 0, 0, 0, 5, 0 },
        { 0, 0, 0, 9, 0, 0, 6, 1, 0 }
    };

    using namespace std::chrono;

    auto start = high_resolution_clock::now();
    Sudoku sudoku(superplus);

    auto end = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end - start);

    std::cout << "耗时: " << duration.count() << " 毫秒" << std::endl;

    sudoku.solution();
    return 1;
}
