#include <stdlib.h>
#include <stdio.h>

#define n 9
int sudoku_solver (int matrix[n][n], int strings[n][n + 1], int columns[n][n + 1], int boxes[n][n + 1]);

int main(void){
    FILE *fl_in = fopen("input.txt", "r");
    FILE *fl_out = fopen("output.txt", "w+");
    fseek(fl_in, 0 , SEEK_END);
    long poss = ftell(fl_in);
    if(poss == 0){ // проверка, что файл пустой
        fprintf(fl_out,"File is empty");
        return 0;
    }
    fseek(fl_in, 0, SEEK_SET); // восстановление указателя на начало файла
    if(fl_in == NULL){
        fprintf(stderr, "FILE CONNECTION ERROR\n");
        return 1;
    }

        int matrix[n][n] = {0}, strings [n][n + 1] = {0}, columns [n][n+ 1] = {0}, boxes [n][n + 1] = {0}; // в трёх последних массивах хранятся данные по принципу
// (индекс массива + 1) = цифра, которая есть в строке/столбце/квадрате, а в последней ячейке хранится количество заполненных ячеек
        char curVal = '\n';
        int curNum;
        while (curVal == '\n') { // считывание всех '\n' (сделано, для того, чтобы не было проблем с вводом данных)
            fscanf(fl_in, "%c", &curVal);
        }
        fseek(fl_in, -1, SEEK_CUR); //в последний раз сработал while и указатель сместился на символ правее, поэтому эта строка нужна, чтобы вернуть указатель на нужное место
        int i = 0, j = 0;
        while(fscanf(fl_in, "%c", &curVal) != EOF){// проходимся по матрице
            if(i == n){ // проверка, что строк не больше 9
                while(fscanf(fl_in, "%c", &curVal) != EOF){
                    if(curVal == '\n' || curVal == '\t' || curVal == ' ') continue;
                    fprintf(fl_out,"Incorrectly input");
                    return 2;
                }
                fseek(fl_in,1,SEEK_CUR);
                if(!feof(fl_in)){ // проверка конца файла
                    break;
                }
            }
            
            if(curVal == '\n'){ // переход на новую строку
                if(j == n){
                    i ++;
                    j = 0;
                    continue;
                }
                else{
                    fprintf(fl_out,"Incorrectly input");
                    return 2;
                }
            }
            if(curVal != '.' && (curVal - '0') > 0 && (curVal - '0') < 10){ // если считано число
                curNum = curVal - '0';
                matrix[i][j] = curNum; //запись числа в таблицу
                strings[i][curNum - 1] = 1; // показываем, что такое число есть в этой строке
                strings[i][n] ++;// увеличиваем количество заполненных ячеек в строке
                columns[j][curNum - 1] = 1;// показываем, что такое число есть в этом столбце
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
                boxes[box_ind][curNum - 1] = 1;// показываем, что такое число есть в этом квадрате
                boxes[box_ind][n]++; // увеличиваем количество заполненных ячеек в квадрате
            }
            else if (curVal != '.'){
                fprintf(fl_out,"Incorrectly input");
                return 2;
            }
            j++;
        }

        int res = sudoku_solver(matrix,strings,columns,boxes); // решаем судоку
        if(res == 0){ // если нет решений
            fprintf(fl_out,"Sudoku has no solutions\n\n");
            return 3;
        }
        else if ( res == 2){ // если в input.txt уже решенная судоку
            fprintf(fl_out,"Sudoku has already been solved");
            return 4;
        }

        for(int k = 0; k < n; k++){ // вывод решенного судоку
            for(int l = 0; l < n; l++){
                fprintf(fl_out,"%d",matrix[k][l]);
            }
            fprintf(fl_out,"\n");
        }

    fclose(fl_in);
    fclose(fl_out);
    return 1;
}

int sudoku_solver (int matrix[n][n], int strings[n][n + 1], int columns[n][n + 1], int boxes[n][n + 1]){
    int tmp_maxSum = -10, type  = 0, maxIndex = -10, total_filled_cells = 0;
/* tmp_maxSum - переменная для того, чтобы понять максимальное количество заполненных ячеек (меньше 9)
 * type  - переменная для того, чтобы понять где больше всего заполнено ячеек (в строке, в столбце или в квадрате)
 * maxIndex - индекс объекта, где больше всего заполнено ячеек
 * total_filled_cells - количество заполненных ячеек     */

    for(int i = 0; i < n; i ++){ // поиск tmp_maxSum, type и maxIndex
        total_filled_cells += boxes[i][n]; // считаем, сколько ячеек заполнено в таблице, если будет 80, то это последняя итерация
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
    if(total_filled_cells == 81) return 2; // если судоку уже решено


    int mainI = 0, mainJ = 0, maxSum = 0; /*
 * mainI и mainJ - индексы строки и столбца соответственно, в которых заполнено больше всего ячеек
 * maxSum - максимальная сумма заполненных ячеек по строке и столбцу
 */

    switch(type) // в зависимости от объекта(строка, столбец или квадрат) выбираем где проверять
    {
        case 1: { //если это квадрат, то записываем в tmp_I и tmp_J левую верхнюю точку нужного нам квадрата
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
            for(int i = tmp_I; i < tmp_I + 3; i++){ //проходимся по этому квадрату и ищем клетку, в которой строке и столбце больше всего чисел уже заполнено
                for(int j = tmp_J; j < tmp_J + 3; j++){
                    if(matrix[i][j] == 0){ // если в этой ячейке нет числа
                        if(strings[i][n] + columns[j][n] > maxSum){ // если в этой строке и стобце наибольшее число заполненных клеток
                            mainI = i;
                            mainJ = j;
                            maxSum = strings[i][n] + columns[j][n];
                        }
                    }
                }
            }
            break;
        }
        case 2: { // если это строка
            for (int j = 0; j < n; j++) {
                if (matrix[maxIndex][j] == 0) { // если в этой ячейке нет числа
                    if (boxes[maxIndex / 3 * 3 + j / 3][n] + columns[j][n] > maxSum) { // если в этой строке наибольшее число заполненных клеток
                        mainI = maxIndex;
                        mainJ = j;
                        maxSum = boxes[maxIndex / 3 * 3 + j / 3][n] + columns[j][n];
                    }
                }
            }
            break;
        }
        default: {
            for (int i = 0; i < n; i++) { // если это столбец
                if (matrix[i][maxIndex] == 0) { // если в этой ячейке нет числа
                    if (boxes[maxIndex / 3 + i / 3 * 3][n] + strings[i][n] > maxSum) { // если в этом стобце наибольшее число заполненных клеток
                        mainI = i;
                        mainJ = maxIndex;
                        maxSum = boxes[maxIndex / 3 + i / 3 * 3][n] + strings[i][n];
                    }
                }
            }
            break;
        }
    }

    int box_index; //индекс нужного квадрата
    if(mainI <= 2 && mainJ <= 2) box_index = 0;
    else if (mainI <= 2 && mainJ >= 3 && mainJ <= 5) box_index = 1;
    else if(mainI <= 2 && mainJ >= 6) box_index = 2;
    else if(mainI >= 3 && mainI <= 5 && mainJ <= 2) box_index = 3;
    else if (mainI >= 3 && mainI <= 5 && mainJ >= 3 && mainJ <= 5) box_index = 4;
    else if(mainI >= 3 && mainI <= 5 && mainJ >= 6) box_index = 5;
    else if(mainI >= 6 && mainJ <= 2) box_index = 6;
    else if (mainI >= 6 && mainJ >= 3 && mainJ <= 5) box_index = 7;
    else box_index = 8;

    for(int i = 0; i < n; i++){ // ищем число, которое можно вставить
        if(boxes[box_index][i] == 0 && strings[mainI][i] == 0 && columns[mainJ][i] == 0){ // если этого числа нет в строке, столбце и квадрате
            matrix[mainI][mainJ] = i + 1;
            strings[mainI][i] = 1;
            strings[mainI][n] ++;
            columns[mainJ][i] = 1;
            columns[mainJ][n] ++;
            boxes[box_index][i]  = 1;
            boxes[box_index][n] ++;
            if(total_filled_cells == 80){ //выход из рекурсии, если у нас заполнены все ячейки
                return 1;
            }
            int result = sudoku_solver(matrix,strings,columns,boxes);
            if(result){
                return 1;
            }
            //если это число не подошло, то восстанавливаем то, что было без этого числа
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
