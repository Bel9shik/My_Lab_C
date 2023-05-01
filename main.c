#include <stdlib.h>
#include <stdio.h>

#define n 9

enum SudokuResults{
    NO_SOLUTIONS, // 0
    OK, // 1
    SUDOKU_ALREADY_SOLVED, // 2
    INCORRECTLY_INPUT, // 3
    FILE_ACCESS_ERROR, //4
    DONT_ENOUGH_MEMORY //5
};

int sudoku_solver (int** matrix, int** rows, int** columns, int** boxes);
int main(void){
    FILE *fl_in = fopen("input.txt", "r");
    if(fl_in == NULL){
        fprintf(stderr, "FILE CONNECTION ERROR\n");
        return FILE_ACCESS_ERROR;
    }
    FILE *fl_out = fopen("output.txt", "w+");
    fseek(fl_in, 0 , SEEK_END);
    long poss = ftell(fl_in);
    // если файл пустой
    if(poss == 0){
        fprintf(fl_out,"File is empty");
        return FILE_ACCESS_ERROR;
    }
    fseek(fl_in, 0, SEEK_SET); // восстановление указателя на начало файла
    
    int** matrix = calloc(n, sizeof(int *));
    if(matrix == NULL){
        free(matrix);
        printf("Can't create the array. Don't enough memory ?");
        return DONT_ENOUGH_MEMORY;
    }
    for(int i = 0; i < n; i++){
        matrix[i] = calloc(n, sizeof(int));
        if(matrix[i] == NULL){
            for(int j = 0; j != i; j++)free(matrix[j]);
            free(matrix);
            printf("Can't create the array. Don't enough memory ?");
            return DONT_ENOUGH_MEMORY;
        }
    }

    int** rows = calloc(n, sizeof(int *));
    if(rows == NULL){
        free(rows);
        for(int k = 0; k < n; k++)free(matrix[k]);
        free(matrix);
        printf("Can't create the array. Don't enough memory ?");
        return DONT_ENOUGH_MEMORY;
    }
    for(int i = 0; i < n; i++){
        rows[i] = calloc(n + 1, sizeof(int));
        if(rows[i] == NULL){
            for(int k = 0; k < n; k++)free(matrix[k]);
            free(matrix);
            for(int j = 0; j != i; j++)free(rows[j]);
            free(rows);
            printf("Can't create the array. Don't enough memory ?");
            return DONT_ENOUGH_MEMORY;
        }
    }

    int** columns = calloc(n, sizeof(int *));
    if(columns == NULL){
        free(columns);
        for(int k = 0; k < n; k++)free(matrix[k]);
        free(matrix);
        for(int j = 0; j < n ; j++)free(rows[j]);
        free(rows);
        printf("Can't create the array. Don't enough memory ?");
        return DONT_ENOUGH_MEMORY;
    }
    for(int i = 0; i < n; i++){
        columns[i] = calloc(n + 1, sizeof(int));
        if(columns[i] == NULL){
            for(int k = 0; k < n; k++)free(matrix[k]);
            free(matrix);
            for(int j = 0; j < n; j++)free(rows[j]);
            free(rows);
            for(int j = 0; j != i; j++)free(columns[j]);
            free(columns);
            printf("Can't create the array. Don't enough memory ?");
            return DONT_ENOUGH_MEMORY;
        }
    }

    int** boxes = calloc(n, sizeof(int *));
    if(boxes == NULL){
        free(boxes);
        for(int k = 0; k < n; k++)free(matrix[k]);
        free(matrix);
        for(int j = 0; j < n; j++)free(rows[j]);
        free(rows);
        for(int j = 0; j < n; j++)free(columns[j]);
        free(columns);
        printf("Can't create the array. Don't enough memory ?");
        return DONT_ENOUGH_MEMORY;
    }
    for(int i = 0; i < n; i++){
        boxes[i] = calloc(n + 1, sizeof(int));
        if(boxes[i] == NULL){
            for(int k = 0; k < n; k++)free(matrix[k]);
            free(matrix);
            for(int j = 0; j < n; j++)free(rows[j]);
            free(rows);
            for(int j = 0; j < n; j++)free(columns[j]);
            free(columns);
            for(int j = 0; j != i; j++)free(boxes[j]);
            free(boxes);
            printf("Can't create the array. Don't enough memory ?");
            return DONT_ENOUGH_MEMORY;
        }
    }
    // в трёх последних массивах хранятся данные по принципу
// (индекс массива + 1) = цифра, которая есть в строке/столбце/квадрате, а в последней ячейке хранится количество заполненных ячеек
    char curVal = '\n';
    int curNum;
    // считывание всех '\n' (сделано, для того, чтобы не было проблем с вводом данных)
        while (curVal == '\n') {
            fscanf(fl_in, "%c", &curVal);
        }
    //в последний раз сработал while и указатель сместился на символ правее, поэтому эта строка нужна, чтобы вернуть указатель на нужное место
        fseek(fl_in, -1, SEEK_CUR);
        int i = 0, j = 0;
    // проходимся по матрице
        while(fscanf(fl_in, "%c", &curVal) != EOF){
            // проверка, что строк не больше 9
            if(i == n){
                //обработка ввода после 9 строки в input.txt(если это не '\t' или '\n' или ' ', то
                if(curVal == '\n' || curVal == '\t' || curVal == ' ') {}
                // неверный ввод
                else {
                    fprintf(fl_out,"Incorrectly input");
                    return INCORRECTLY_INPUT;
                }
                while(fscanf(fl_in, "%c", &curVal) != EOF){
                    if(curVal == '\n' || curVal == '\t' || curVal == ' ') continue;
                    fprintf(fl_out,"Incorrectly input");
                    return INCORRECTLY_INPUT;
                }
                fseek(fl_in,1,SEEK_CUR);
                if(!feof(fl_in)){
                    break;
                }
            }

            // переход на новую строку
            if(curVal == '\n'){
                if(j == n){
                    i ++;
                    j = 0;
                    continue;
                }
                else{
                    fprintf(fl_out,"Incorrectly input");
                    return INCORRECTLY_INPUT;
                }
            }
            // если считано число
            if(curVal != '.' && (curVal - '0') > 0 && (curVal - '0') < 10){
                curNum = curVal - '0';
                matrix[i][j] = curNum; //запись числа в таблицу
                rows[i][curNum - 1] ++; // показываем, что такое число есть в этой строке
                rows[i][n] ++;// увеличиваем количество заполненных ячеек в строке
                columns[j][curNum - 1] ++;// показываем, что такое число есть в этом столбце
                columns[j][n] ++; // увеличиваем количество заполненных ячеек в столбце
                int box_ind; //if's снизу для того, чтобы понять в каком квадрате мы находимся (поле поделено на 9 квадратов)
                if(i <= 2 && j <= 2) box_ind = 0;
                else if(i <= 2 && j >= 3 && j <= 5) box_ind = 1;
                else if(i <= 2 && j >= 6) box_ind = 2;
                else if(i >= 3 && i <= 5 && j <= 2) box_ind = 3;
                else if(i >= 3 && i <= 5 &&  j >= 3 && j <= 5) box_ind = 4;
                else if(i >= 3 && i <= 5 && j >= 6) box_ind = 5;
                else if(i >= 6 && j <= 2) box_ind = 6;
                else if(i >= 6 && j >= 3 && j <= 5) box_ind = 7;
                else box_ind = 8;
                boxes[box_ind][curNum - 1] ++;// показываем, что такое число есть в этом квадрате
                boxes[box_ind][n] ++; // увеличиваем количество заполненных ячеек в квадрате
            }
            else if (curVal != '.'){
                fprintf(fl_out,"Incorrectly input");
                return INCORRECTLY_INPUT;
            }
            j++;
        }
    //проверка на одинаковые числа в строках / столбцах / квадратах
        for(int k = 0; k < n; k++ ){
            for(int l = 0; l < n; l++){
                if(boxes[k][l] > 1 || rows[k][l] > 1 || columns[k][l] > 1){
                    fprintf(fl_out,"Incorrectly input");
                    return INCORRECTLY_INPUT;
                }
            }
        }
    
        // если нет решений
        int res = sudoku_solver(matrix, rows, columns, boxes);
        if(res == NO_SOLUTIONS){
            fprintf(fl_out,"Sudoku has no solutions\n\n");
            return NO_SOLUTIONS;
        }
        // если на входе дана уже решенная судоку
        else if ( res == SUDOKU_ALREADY_SOLVED){
            fprintf(fl_out,"Sudoku has already been solved");
            return SUDOKU_ALREADY_SOLVED;
        }
        // вывод решенного судоку
        for(int k = 0; k < n; k++){
            for(int l = 0; l < n; l++){
                fprintf(fl_out,"%d",matrix[k][l]);
            }
            fprintf(fl_out,"\n");
        }
        //освобождение памяти
        for(int k = 0; k < n; k ++){
            free(rows[k]);
            free(columns[k]);
            free(boxes[k]);
            free(matrix[k]);
        }
        free(matrix);
        free(rows);
        free(columns);
        free(boxes);

    fclose(fl_in);
    fclose(fl_out);
    return OK;
}

int sudoku_solver (int** matrix, int** rows, int** columns, int** boxes){
    int tmp_maxSum = -10, type  = 0, maxIndex = -10, total_filled_cells = 0;
/* tmp_maxSum - переменная для того, чтобы понять максимальное количество заполненных ячеек (меньше 9)
 * type  - переменная для того, чтобы понять где больше всего заполнено ячеек (в строке, в столбце или в квадрате)
 * maxIndex - индекс объекта, где больше всего заполнено ячеек
 * total_filled_cells - количество заполненных ячеек     */

    // поиск tmp_maxSum, type и maxIndex
    for(int i = 0; i < n; i ++){
        total_filled_cells += boxes[i][n]; // считаем, сколько ячеек заполнено в таблице, если будет 80, то это последняя итерация
        if(boxes[i][n] > tmp_maxSum && boxes[i][n] < n){
            type = 1;
            maxIndex = i;
            tmp_maxSum = boxes[i][n];
        }
        if(rows[i][n] > tmp_maxSum && rows[i][n] < n){
            type = 2;
            maxIndex= i;
            tmp_maxSum = rows[i][n];
        }
        if(columns[i][n] > tmp_maxSum && columns[i][n] < n){
            type = 3;
            maxIndex = i;
            tmp_maxSum = columns[i][n];
        }
    }

    // если судоку уже решено
    if(total_filled_cells == 81) return SUDOKU_ALREADY_SOLVED;

/*
 * mainI и mainJ - индексы строки и столбца соответственно, в которых заполнено больше всего ячеек
 * maxSum - максимальная сумма заполненных ячеек по строке и столбцу
 */

    int mainI = 0, mainJ = 0, maxSum = 0;

    // в зависимости от объекта(строка, столбец или квадрат) выбираем где проверять
    switch(type)
    {
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
            for(int i = tmp_I; i < tmp_I + 3; i++){
                for(int j = tmp_J; j < tmp_J + 3; j++){
                    // если в этой ячейке нет числа
                    if(matrix[i][j] == 0){
                        // если в этой строке и стобце наибольшее число заполненных клеток
                        if(rows[i][n] + columns[j][n] > maxSum){
                            mainI = i;
                            mainJ = j;
                            maxSum = rows[i][n] + columns[j][n];
                        }
                    }
                }
            }
            break;
        }
            // если это строка
        case 2: {
            for (int j = 0; j < n; j++) {
                if (matrix[maxIndex][j] == 0) { // если в этой ячейке нет числа
                    // если в этой строке наибольшее число заполненных клеток
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
            // если это столбец
            for (int i = 0; i < n; i++) {
                // если в этой ячейке нет числа
                if (matrix[i][maxIndex] == 0) {
                    // если в этом стобце наибольшее число заполненных клеток
                    if (boxes[maxIndex / 3 + i / 3 * 3][n] + rows[i][n] > maxSum) {
                        mainI = i;
                        mainJ = maxIndex;
                        maxSum = boxes[maxIndex / 3 + i / 3 * 3][n] + rows[i][n];
                    }
                }
            }
            break;
        }
    }

    //индекс нужного квадрата
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

    // ищем число, которое можно вставить
    for(int i = 0; i < n; i++){
        // если этого числа нет в строке, столбце и квадрате
        if(boxes[box_index][i] == 0 && rows[mainI][i] == 0 && columns[mainJ][i] == 0){
            matrix[mainI][mainJ] = i + 1;
            columns[mainJ][n] ++;
            boxes[box_index][i]  = 1;
            boxes[box_index][n] ++;
            //выход из рекурсии, если у нас заполнены все ячейки
            if(total_filled_cells == 80){
                return OK;
            }
            rows[mainI][i] = 1;
            rows[mainI][n] ++;
            columns[mainJ][i] = 1;
            int result = sudoku_solver(matrix, rows, columns, boxes);
            if(result){
                return OK;
            }
            //если это число не подошло, то восстанавливаем то, что было без этого числа
            matrix[mainI][mainJ] = 0;
            rows[mainI][i] = 0;
            rows[mainI][n] --;
            columns[mainJ][i] = 0;
            columns[mainJ][n] --;
            boxes[box_index][i]  = 0;
            boxes[box_index][n] --;
        }
    }
    return NO_SOLUTIONS;
}
