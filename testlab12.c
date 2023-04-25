#include "testLab.h"
#include <stdio.h>

#define TESTS_COUNT 2
#define TIME_LIMIT 1000
#define MEMORY_LIMIT (1 << 10)

typedef struct IOStream IOStream;
typedef struct TTestPackage TTestPackage;
typedef struct TTestInOut TTestInOut;
typedef struct TTestStrings TTestStrings;

struct TTestStrings{
    char string [9];
};

struct TTestPackage {
    TTestStrings Input[9];
    TTestStrings Result[9];
//    size_t CountStrings;
    size_t ResultCountStrings;
};

struct TTestInOut{
    TTestPackage Tests [TESTS_COUNT];
};

struct IOStream {
    FILE* In;
    FILE* Out;
};

static TTestInOut TestData = {
        {{{
            {"..3.2.6..",}, {"9..3.5..1"}, {"..18.64.."}, {"..81.29.."}, {"7.......8"}, {"..67.82.."}, {"..26.95.."},{"8..2.3..9"}, {"..5.1.3.."}
        },
          {
            {"483921657"},{"967345821"}, {"251876493"},{"548132976"}, {"729564138"},{"136798245"},{"372689514"},{"814253769"},{"695417382"}},
            9
        },
         {{
                  {"..18....5"},{"4...35.2."},{".3.7....."},{"..6....4."},{".8..91..2"},{"...5....."},{"....7...."},{".9..23..1"},{"8.....9.."}

         },{
                 {"261849735"},{"479635128"},{"538712496"},{"156287349"},{"784391652"},{"923564817"},{"315978264"},{"697423581"},{"842156973"}
         }, 9
         },
//         {{
//                  {"61....8.3"},{".47.8.5.."},{"...9....6"},{"...3...14"},{"324.16.58"},{"56.498.72"},{".365.9..."},{"...a67..5"},{"7.....4.."}
//         }, {
//                 {"Incorrectly input"}
//         }
//         }
    }
};

static size_t currentTest = 0;

IOStream stream = {NULL, NULL};

static int FeedFromArray(void){
    stream.In = fopen("input.txt", "w+");
    if(!stream.In){
        printf("Can't create the file. Don't space on disk ?");
        return -1;
    }
    for (size_t i = 0; i < 9; i++){
        fprintf(stream.In, "%s\n", TestData.Tests[currentTest].Input[i].string);
    }
    fclose(stream.In);
    return 0;
}

size_t readStringCount = 0;

static int CheckFromArray(void){
    stream.Out = fopen("output.txt","r");
    if(!stream.Out){
        printf("Can't open output.txt");
        currentTest++;
        return -1;
    }

    char OutString[9] = {0};
    while(!fscanf(stream.Out,"%s",OutString)){
        if(readStringCount >= TestData.Tests[currentTest].ResultCountStrings){
            printf("Output is long -- FAILED\n");
            return 1;
        }

        if(TestData.Tests[currentTest].Result[readStringCount].string[0] != OutString[0] || TestData.Tests[currentTest].Result[readStringCount].string[1] != OutString[1] || TestData.Tests[currentTest].Result[readStringCount].string[2] != OutString[2] || TestData.Tests[currentTest].Result[readStringCount].string[3] != OutString[3] || TestData.Tests[currentTest].Result[readStringCount].string[4] != OutString[4] || TestData.Tests[currentTest].Result[readStringCount].string[5] != OutString[5] || TestData.Tests[currentTest].Result[readStringCount].string[6] != OutString[6] || TestData.Tests[currentTest].Result[readStringCount].string[7] != OutString[7] || TestData.Tests[currentTest].Result[readStringCount].string[8] != OutString[8]){
            printf("Wrong answer -- FAILED");
            return 1;
        }
        readStringCount ++;
    }

        if(readStringCount < TestData.Tests[currentTest].ResultCountStrings){
            printf("Output is short -- FAILED");
            return 1;
        }

    printf("PASSED");
    currentTest ++;;
    fclose(stream.Out);
    return 0;
}

const TLabTest labTests[] = {
        {FeedFromArray,CheckFromArray}
};

TLabTest GiveLabTest (int testInd){
    (void) testInd;
    TLabTest labTest = {FeedFromArray, CheckFromArray};
    return labTest;
}

int GiveTestLabCount(void){
    return TESTS_COUNT;
}

const char* GetTesterName(void){
    return "Lab 12 Sudoku solver";
}

int GiveTestTimeout(void){
    return TIME_LIMIT;
}

size_t GiveTestMemoryLimit(void){
    return MIN_PROCESS_RSS_BYTES + MEMORY_LIMIT;
}
