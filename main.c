#include <stdlib.h>
#include <stdio.h>

#define n 9
int sudoku_solver (int matrix[n][n], int strings[n][n + 1], int columns[n][n + 1], int boxes[n][n + 1]);

int main(void){
    FILE *fl_in = fopen("input.txt", "r");
    FILE *fl_out = fopen("output.txt", "w");
    if(fl_in == NULL){
        fprintf(stderr, "FILE CONNECTION ERROR\n");
        return 1;
    }
    int repeats = 0;

    fscanf(fl_in,"%d\n",&repeats);

    for(int r = 0; r < repeats; r ++){
        int matrix[n][n] = {0}, strings [n][n + 1] = {0}, columns [n][n+ 1] = {0}, boxes [n][n + 1] = {0};
        char curVal = '\n';
        int curNum;
        while (curVal == '\n') {
            fscanf(fl_in, "%c", &curVal);
        }
        fseek(fl_in, -1, SEEK_CUR);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                fscanf(fl_in, "%c", &curVal);
                if(curVal != '.'){
                    curNum = curVal - '0';
                    matrix[i][j] = curNum;
                    strings[i][curNum - 1] = 1;
                    strings[i][n] ++;
                    columns[j][curNum - 1] = 1;
                    columns[j][n] ++;
                    int box_ind;
                    if(i <= 2 && j <= 2) box_ind = 0;
                    else if(i <= 2 && j >= 3 && j <= 5) box_ind = 1;
                    else if(i <= 2 && j >= 6) box_ind = 2;
                    else if(i >= 3 && i <= 5 && j <= 2) box_ind = 3;
                    else if(i >= 3 && i <= 5 &&  j >= 3 && j <= 5) box_ind = 4;
                    else if(i >= 3 && i <= 5 && j >= 6) box_ind = 5;
                    else if(i >= 6 && j <= 2) box_ind = 6;
                    else if(i >= 6 && j >= 3 && j <= 5) box_ind = 7;
                    else box_ind = 8;
                    boxes[box_ind][curNum - 1] = 1;
                    boxes[box_ind][n]++;
                }
            }
            fscanf(fl_in, "\n");
        }

        int res = sudoku_solver(matrix,strings,columns,boxes);
        if(res == 0){
            fprintf(fl_out,"Судоку не решается\n\n");
            continue;
        }

        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                fprintf(fl_out,"%d",matrix[i][j]);
            }
            fprintf(fl_out,"\n");
        }
        fprintf(fl_out, "\n");
    }

    fclose(fl_in);
    fclose(fl_out);
    return 1;
}

int sudoku_solver (int matrix[n][n], int strings[n][n + 1], int columns[n][n + 1], int boxes[n][n + 1]){
    int tmp_maxSum = -10, type  = 0, maxIndex = -10, total_filled_cells = 0;

    for(int i = 0; i < n; i ++){
        total_filled_cells += boxes[i][n];
        if(boxes[i][n] > tmp_maxSum && boxes[i][n] < n){
            type = 1;
            maxIndex = i;
            tmp_maxSum = boxes[i][n];
        }
        if(strings[i][n] > tmp_maxSum && strings[i][n] < n){
            type = 2;
            maxIndex= i;
            tmp_maxSum = strings[i][n];
        }
        if(columns[i][n] > tmp_maxSum && columns[i][n] < n){
            type = 3;
            maxIndex = i;
            tmp_maxSum = columns[i][n];
        }
    }

    int mainI = 0, mainJ = 0, maxSum = 0;

    switch(type)
    {
        case 1: {
            int tmp_I, tmp_J;
            switch (maxIndex) {
                case 0:
                    tmp_I = 0;
                    tmp_J = 0;
                    break;
                case 1:
                    tmp_I = 0;
                    tmp_J = 3;
                    break;
                case 2:
                    tmp_I = 0;
                    tmp_J = 6;
                    break;
                case 3:
                    tmp_I = 3;
                    tmp_J = 0;
                    break;
                case 4:
                    tmp_I = 3;
                    tmp_J = 3;
                    break;
                case 5:
                    tmp_I = 3;
                    tmp_J = 6;
                    break;
                case 6:
                    tmp_I = 6;
                    tmp_J = 0;
                    break;
                case 7:
                    tmp_I = 6;
                    tmp_J = 3;
                    break;
                default:
                    tmp_I = 6;
                    tmp_J = 6;
                    break;
            }
            for(int i = tmp_I; i < tmp_I + 3; i++){
                for(int j = tmp_J; j < tmp_J + 3; j++){
                    if(matrix[i][j] == 0){
                        if(strings[i][n] + columns[j][n] > maxSum){
                            mainI = i;
                            mainJ = j;
                            maxSum = strings[i][n] + columns[j][n];
                        }
                    }
                }
            }
            break;
        }
        case 2: {
            for (int j = 0; j < n; j++) {
                if (matrix[maxIndex][j] == 0) {
                    if (boxes[maxIndex / 3 * 3 + j / 3][n] + columns[j][n] > maxSum) {
                        mainI = maxIndex;
                        mainJ = j;
                        maxSum = boxes[maxIndex / 3 * 3 + j / 3][n] + columns[j][n];
                    }
                }
            }
            break;
        }
        default: {
            for (int i = 0; i < n; i++) {
                if (matrix[i][maxIndex] == 0) {
                    if (boxes[maxIndex / 3 + i / 3 * 3][n] + strings[i][n] > maxSum) {
                        mainI = i;
                        mainJ = maxIndex;
                        maxSum = boxes[maxIndex / 3 + i / 3 * 3][n] + strings[i][n];
                    }
                }
            }
            break;
        }
    }

    int box_index;
    if(mainI <= 2 && mainJ <= 2) box_index = 0;
    else if (mainI <= 2 && mainJ >= 3 && mainJ <= 5) box_index = 1;
    else if(mainI <= 2 && mainJ >= 6) box_index = 2;
    else if(mainI >= 3 && mainI <= 5 && mainJ <= 2) box_index = 3;
    else if (mainI >= 3 && mainI <= 5 && mainJ >= 3 && mainJ <= 5) box_index = 4;
    else if(mainI >= 3 && mainI <= 5 && mainJ >= 6) box_index = 5;
    else if(mainI >= 6 && mainJ <= 2) box_index = 6;
    else if (mainI >= 6 && mainJ >= 3 && mainJ <= 5) box_index = 7;
    else box_index = 8;

    for(int i = 0; i < n; i++){
        if(boxes[box_index][i] == 0 && strings[mainI][i] == 0 && columns[mainJ][i] == 0){
            matrix[mainI][mainJ] = i + 1;
            strings[mainI][i] = 1;
            strings[mainI][n] ++;
            columns[mainJ][i] = 1;
            columns[mainJ][n] ++;
            boxes[box_index][i]  = 1;
            boxes[box_index][n] ++;
            if(total_filled_cells == 80){
                return 1;
            }
            int result = sudoku_solver(matrix,strings,columns,boxes);
            if(result){
                return 1;
            }
            matrix[mainI][mainJ] = 0;
            strings[mainI][i] = 0;
            strings[mainI][n] --;
            columns[mainJ][i] = 0;
            columns[mainJ][n] --;
            boxes[box_index][i]  = 0;
            boxes[box_index][n] --;
        }
    }
    return 0;
}
