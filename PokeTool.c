#include <stdio.h>
#include <math.h>
#include "PokeTool.h"

// Line Print
void getLine(int n) {
    for(int i = 1; i <= n; i++) printGrad("=");
    printf("\n");
}

// Terminal Gradation Text Color
void printGrad(const char* str) {
    int r, g, b;
    double freq = 0.05;
    for(int i = 0; str[i] != '\0';) {
        r = (sin(freq * i + 0) * 127) + 128;
        g = (sin(freq * i + 1) * 127) + 128;
        b = (sin(freq * i + 2) * 127) + 128;
        if ((str[i] & 0x80) != 0) {
            printf("\033[38;2;%d;%d;%dm%.3s", r, g, b, &str[i]);
            i += 3;
        } else {
            printf("\033[38;2;%d;%d;%dm%c", r, g, b, str[i]);
            i++;
        }
    }
    printf("\033[0m");
}

// Input the Only Number
int onlyNumber(int *num) {
    if (scanf("%d", num) != 1) {
        getLine(DEF_LINE_NUM); printf(RED "숫자만 입력해주세요.\n" RESET);
        while (getchar() != '\n') continue;
        return 1;
    }
    return 0;
}