#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define SUDOKU_SIZE 9

enum SudokuResults{
    NO_SOLUTIONS,
    OK,
    SUDOKU_ALREADY_SOLVED
};

int sudoku_solver(char** matrix, char** rows, char** columns, char** boxes);
int count_solve(int row, int col, char** matrix, int count);
int legal(int row, int col, int val, const char** matrix);
void free_resources(char** matrix, char** rows, char** columns, char** boxes);

int main(void){

    FILE *fl_in = fopen("in.txt", "r");
    if(fl_in == NULL){
        printf("FILE CONNECTION ERROR\n");
        return EIO;
    }
    fseek(fl_in, 0 ,SEEK_END);
    long poss = ftell(fl_in);
    // если файл пустой
    if (poss == 0){
        printf("File is empty");
        return EIO;
    }
    fseek(fl_in, 0, SEEK_SET); // восстановление указателя на начало файла

    char** matrix = calloc(SUDOKU_SIZE, sizeof(char *));
    if (matrix == NULL) {
        printf("Can't create the array. Not enough memory ?");
        fclose(fl_in);
        return ENOMEM;
    }
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        matrix[i] = calloc(SUDOKU_SIZE, sizeof(char));
        if (matrix[i] == NULL) {
            free_resources(matrix, NULL, NULL, NULL);
            printf("Can't create array. Not enough memory ?");
            fclose(fl_in);
            return ENOMEM;
        }
    }

    char** rows = calloc(SUDOKU_SIZE, sizeof(char *));
    if (rows == NULL) {
        free_resources(matrix, NULL, NULL, NULL);
        printf("Can't create array. Not enough memory ?");
        fclose(fl_in);
        return ENOMEM;
    }
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        rows[i] = calloc(SUDOKU_SIZE + 1, sizeof(char));
        if (rows[i] == NULL) {
            free_resources(matrix, rows, NULL, NULL);
            printf("Can't create array. Not enough memory ?");
            fclose(fl_in);
            return ENOMEM;
        }
    }

    char** columns = calloc(SUDOKU_SIZE, sizeof(char *));
    if (columns == NULL) {
        free_resources(matrix, rows, NULL, NULL);
        printf("Can't create array. Not enough memory ?");
        fclose(fl_in);
        return ENOMEM;
    }
    for(int i = 0; i < SUDOKU_SIZE; i++){
        columns[i] = calloc(SUDOKU_SIZE + 1, sizeof(char));
        if(columns[i] == NULL){
            free_resources(matrix, rows, columns, NULL);
            printf("Can't create array. Not enough memory ?");
            fclose(fl_in);
            return ENOMEM;
        }
    }

    char** boxes = calloc(SUDOKU_SIZE, sizeof(char *));
    if(boxes == NULL){
        free_resources(matrix, rows, columns, NULL);
        printf("Can't create array. Not enough memory ?");
        fclose(fl_in);
        return ENOMEM;
    }
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        boxes[i] = calloc(SUDOKU_SIZE + 1, sizeof(char));
        if (boxes[i] == NULL) {
            free_resources(matrix, rows, columns, boxes);
            printf("Can't create array. Not enough memory ?");
            fclose(fl_in);
            return ENOMEM;
        }
    }
    // в трёх последних массивах хранятся данные по принципу
// (индекс массива + 1) = цифра, которая есть в строке/столбце/квадрате, а в последней ячейке хранится количество заполненных ячеек
    char curVal = '\n';
    int curNum;
    // считывание всех '\n' (сделано, для того, чтобы не было проблем с вводом данных)
    while (curVal == '\n') {
        fscanf(fl_in, "%c", &curVal);
        if(feof(fl_in)){
            printf("Incorrect input data");
            free_resources(matrix, rows, columns, boxes);
            fclose(fl_in);
            return 0;
        }
    }
    //в последний раз сработал while и указатель сместился на символ правее, поэтому эта строка нужна, чтобы вернуть указатель на нужное место
    fseek(fl_in, -1, SEEK_CUR);
    int i = 0, j = 0;
    // проходимся по матрице
    while (fscanf(fl_in, "%c", &curVal) != EOF) {
        // проверка, что строк не больше 9
        if (i == SUDOKU_SIZE) {
            //обработка ввода после 9 строки в input.txt(если это не '\t' или '\n' или ' ', то
            if (curVal == '\n' || curVal == '\t' || curVal == ' ') {}
                // неверный ввод
            else {
                printf("Incorrect input data");
                free_resources(matrix, rows, columns, boxes);
                fclose(fl_in);
                return 0;
            }
            while (fscanf(fl_in, "%c", &curVal) != EOF) {
                if (curVal == '\n' || curVal == '\t' || curVal == ' ') continue;
                printf("Incorrect input data");
                free_resources(matrix, rows, columns, boxes);
                fclose(fl_in);
                return 0;
            }
        }

        // переход на новую строку
        if (curVal == '\n') {
            if (j == SUDOKU_SIZE) {
                i ++;
                j = 0;
                continue;
            } else {
                printf("Incorrect input data");
                free_resources(matrix, rows, columns, boxes);
                fclose(fl_in);
                return 0;
            }
        }
        // если считано число
        if (curVal != '.' && (curVal - '0') > 0 && (curVal - '0') < 10) {
            curNum = curVal - '0';
            matrix[i][j] = (char)curNum; //запись числа в таблицу
            rows[i][curNum - 1]++; // показываем, что такое число есть в этой строке
            rows[i][SUDOKU_SIZE]++;// увеличиваем количество заполненных ячеек в строке
            columns[j][curNum - 1]++;// показываем, что такое число есть в этом столбце
            columns[j][SUDOKU_SIZE]++; // увеличиваем количество заполненных ячеек в столбце
            int box_ind; //if's снизу для того, чтобы понять в каком квадрате мы находимся (поле поделено на 9 квадратов)
            if (i <= 2 && j <= 2) box_ind = 0;
            else if (i <= 2 && j >= 3 && j <= 5) box_ind = 1;
            else if (i <= 2 && j >= 6) box_ind = 2;
            else if (i >= 3 && i <= 5 && j <= 2) box_ind = 3;
            else if (i >= 3 && i <= 5 &&  j >= 3 && j <= 5) box_ind = 4;
            else if (i >= 3 && i <= 5 && j >= 6) box_ind = 5;
            else if (i >= 6 && j <= 2) box_ind = 6;
            else if (i >= 6 && j >= 3 && j <= 5) box_ind = 7;
            else box_ind = 8;
            boxes[box_ind][curNum - 1]++;// показываем, что такое число есть в этом квадрате
            boxes[box_ind][SUDOKU_SIZE]++; // увеличиваем количество заполненных ячеек в квадрате
        } else if (curVal != '.'){
            printf("Incorrect input data");
            free_resources(matrix, rows, columns, boxes);
            fclose(fl_in);
            return 0;
        }
        j++;
    }
    //проверка на одинаковые числа в строках / столбцах / квадратах
    for (int k = 0; k < SUDOKU_SIZE; k++) {
        for (int l = 0; l < SUDOKU_SIZE; l++) {
            if (boxes[k][l] > 1 || rows[k][l] > 1 || columns[k][l] > 1) {
                printf("Incorrect input data");
                free_resources(matrix, rows, columns, boxes);
                fclose(fl_in);
                return 0;
            }
        }
    }
    //проверка на количество решений
    int count = 0;
    i = 0, j = 0;
    int count_solutions = count_solve(i,j,matrix,count);
    if (count_solutions > 1) {
        printf("Sudoku has more than 1 solution");
        free_resources(matrix, rows, columns, boxes);
        fclose(fl_in);
        return 0;
    }

    // если нет решений
    int res = sudoku_solver(matrix, rows, columns, boxes);
    if (res == NO_SOLUTIONS) {
        printf("No solutions");
        free_resources(matrix, rows, columns, boxes);
        fclose(fl_in);
        return 0;
    }
        // если на входе дана уже решенная судоку
    else if (res == SUDOKU_ALREADY_SOLVED) {
        printf("This sudoku already solved");
        free_resources(matrix, rows, columns, boxes);
        fclose(fl_in);
        return 0;
    }
    // вывод решенного судоку
    for (int k = 0; k < SUDOKU_SIZE; k++) {
        for (int l = 0; l < SUDOKU_SIZE; l++) {
            printf("%d",matrix[k][l]);
        }
        if (k == 8) break;
        printf("\n");
    }
    //освобождение памяти
    free_resources(matrix, rows, columns, boxes);
    fclose(fl_in);
    return 0;
}


int sudoku_solver(char** matrix, char** rows, char** columns, char** boxes){
    int tmp_maxSum = -10, type  = 0, maxIndex = -10, total_filled_cells = 0;
/* tmp_maxSum - переменная для того, чтобы понять максимальное количество заполненных ячеек (меньше 9)
 * type  - переменная для того, чтобы понять где больше всего заполнено ячеек (в строке, в столбце или в квадрате)
 * maxIndex - индекс объекта, где больше всего заполнено ячеек
 * total_filled_cells - количество заполненных ячеек     */

    // поиск tmp_maxSum, type и maxIndex
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        total_filled_cells += boxes[i][SUDOKU_SIZE]; // считаем, сколько ячеек заполнено в таблице, если будет 80, то это последняя итерация
        if (boxes[i][SUDOKU_SIZE] > tmp_maxSum && boxes[i][SUDOKU_SIZE] < SUDOKU_SIZE) {
            type = 1;
            maxIndex = i;
            tmp_maxSum = boxes[i][SUDOKU_SIZE];
        }
        if (rows[i][SUDOKU_SIZE] > tmp_maxSum && rows[i][SUDOKU_SIZE] < SUDOKU_SIZE) {
            type = 2;
            maxIndex= i;
            tmp_maxSum = rows[i][SUDOKU_SIZE];
        }
        if (columns[i][SUDOKU_SIZE] > tmp_maxSum && columns[i][SUDOKU_SIZE] < SUDOKU_SIZE) {
            type = 3;
            maxIndex = i;
            tmp_maxSum = columns[i][SUDOKU_SIZE];
        }
    }

    // если судоку уже решено
    if (total_filled_cells == 81) return SUDOKU_ALREADY_SOLVED;

/*
 * mainI и mainJ - индексы строки и столбца соответственно, в которых заполнено больше всего ячеек
 * maxSum - максимальная сумма заполненных ячеек по строке и столбцу
 */

    int mainI = 0, mainJ = 0, maxSum = 0;

    // в зависимости от объекта(строка, столбец или квадрат) выбираем где проверять
    switch (type) {
        //если это квадрат, то записываем в tmp_I и tmp_J левую верхнюю точку нужного нам квадрата
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
            //проходимся по этому квадрату и ищем клетку, в которой строке и столбце больше всего чисел уже заполнено
            for (int i = tmp_I; i < tmp_I + 3; i++) {
                for (int j = tmp_J; j < tmp_J + 3; j++) {
                    // если в этой ячейке нет числа
                    if (matrix[i][j] == 0) {
                        // если в этой строке и стобце наибольшее число заполненных клеток
                        if (rows[i][SUDOKU_SIZE] + columns[j][SUDOKU_SIZE] > maxSum) {
                            mainI = i;
                            mainJ = j;
                            maxSum = rows[i][SUDOKU_SIZE] + columns[j][SUDOKU_SIZE];
                        }
                    }
                }
            }
            break;
        }
            // если это строка
        case 2: {
            for (int j = 0; j < SUDOKU_SIZE; j++) {
                if (matrix[maxIndex][j] == 0) { // если в этой ячейке нет числа
                    // если в этой строке наибольшее число заполненных клеток
                    if (boxes[maxIndex / 3 * 3 + j / 3][SUDOKU_SIZE] + columns[j][SUDOKU_SIZE] > maxSum) {
                        mainI = maxIndex;
                        mainJ = j;
                        maxSum = boxes[maxIndex / 3 * 3 + j / 3][SUDOKU_SIZE] + columns[j][SUDOKU_SIZE];
                    }
                }
            }
            break;
        }
        default: {
            // если это столбец
            for (int i = 0; i < SUDOKU_SIZE; i++) {
                // если в этой ячейке нет числа
                if (matrix[i][maxIndex] == 0) {
                    // если в этом стобце наибольшее число заполненных клеток
                    if (boxes[maxIndex / 3 + i / 3 * 3][SUDOKU_SIZE] + rows[i][SUDOKU_SIZE] > maxSum) {
                        mainI = i;
                        mainJ = maxIndex;
                        maxSum = boxes[maxIndex / 3 + i / 3 * 3][SUDOKU_SIZE] + rows[i][SUDOKU_SIZE];
                    }
                }
            }
            break;
        }
    }

    //индекс нужного квадрата
    int box_index;
    if (mainI <= 2 && mainJ <= 2) box_index = 0;
    else if (mainI <= 2 && mainJ >= 3 && mainJ <= 5) box_index = 1;
    else if (mainI <= 2 && mainJ >= 6) box_index = 2;
    else if (mainI >= 3 && mainI <= 5 && mainJ <= 2) box_index = 3;
    else if (mainI >= 3 && mainI <= 5 && mainJ >= 3 && mainJ <= 5) box_index = 4;
    else if (mainI >= 3 && mainI <= 5 && mainJ >= 6) box_index = 5;
    else if (mainI >= 6 && mainJ <= 2) box_index = 6;
    else if (mainI >= 6 && mainJ >= 3 && mainJ <= 5) box_index = 7;
    else box_index = 8;

    // ищем число, которое можно вставить
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        // если этого числа нет в строке, столбце и квадрате
        if (boxes[box_index][i] == 0 && rows[mainI][i] == 0 && columns[mainJ][i] == 0) {
            matrix[mainI][mainJ] = (char)(i + 1);
            columns[mainJ][SUDOKU_SIZE]++;
            boxes[box_index][i]  = 1;
            boxes[box_index][SUDOKU_SIZE]++;
            //выход из рекурсии, если у нас заполнены все ячейки
            if (total_filled_cells == 80) {
                return OK;
            }
            rows[mainI][i] = 1;
            rows[mainI][SUDOKU_SIZE]++;
            columns[mainJ][i] = 1;
            int result = sudoku_solver(matrix, rows, columns, boxes);
            if (result) {
                return OK;
            }
            //если это число не подошло, то восстанавливаем то, что было без этого числа
            matrix[mainI][mainJ] = 0;
            rows[mainI][i] = 0;
            rows[mainI][SUDOKU_SIZE]--;
            columns[mainJ][i] = 0;
            columns[mainJ][SUDOKU_SIZE]--;
            boxes[box_index][i]  = 0;
            boxes[box_index][SUDOKU_SIZE]--;
        }
    }
    return NO_SOLUTIONS;
}

void free_resources(char** matrix, char** rows, char** columns, char** boxes){
    if (matrix != NULL) {
        for (int i = 0; i < SUDOKU_SIZE && matrix[i] != NULL; i++) free(matrix[i]);
        free(matrix);
    }
    if (rows != NULL) {
        for (int i = 0; i < SUDOKU_SIZE && rows[i] != NULL; i++) free(rows[i]);
        free(rows);
    }
    if (columns != NULL) {
        for (int i = 0; i < SUDOKU_SIZE && columns[i] != NULL; i++) free(columns[i]);
        free(columns);
    }
    if (boxes != NULL) {
        for (int i = 0; i < SUDOKU_SIZE && boxes[i] != NULL; i++) free(boxes[i]);
        free(boxes);
    }
}

int count_solve(int row, int col, char** matrix, int count){

    if (row == SUDOKU_SIZE){
        row = 0;
        if (++col == SUDOKU_SIZE) {
            return count + 1;
        }
    }
    //пропускаем пустые ячейки
    if (matrix[row][col] != 0) return count_solve(row + 1, col, matrix, count);
    //ищем два решения
    //выходим из цикла, если найдём 2 решения
    for (int val = 1; val <= 9 && count < 2; val++) {
        if (legal(row, col, val, (const char **) matrix)) {
            matrix[row][col] = (char)val;
            count = count_solve(row + 1, col, matrix, count);
        }
    }
    matrix[row][col] = 0; // восставниваем ячейку
    return count;
}

int legal(int row, int col, int val, const char** matrix){
    //проверяем строку
    for (int k = 0; k < SUDOKU_SIZE; k++) {
        if (matrix[row][k] == val) return 0;
    }

    //проверяем столбец
    for (int k = 0; k < SUDOKU_SIZE; k++) {
        if (matrix[k][col] == val) return 0;
    }

    //проверяем квадрант
    int tmp_I = (row / 3) * 3;
    int tmp_J = (col / 3) * 3;

    for (int k = 0; k < 3; k++) {
        for (int l = 0; l < 3; l++) {
            if (matrix[tmp_I + k][tmp_J + l] == val) return 0;
        }
    }

    return 1;
}
