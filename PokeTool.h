#pragma once

// Terminal Text Colors
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define GRAY "\033[0;37m"
#define RESET "\033[0m"

// Default Line Number
#define DEF_LINE_NUM 36

// Pokemon Add-on Functions
void getLine(int n);
void printGrad(const char* str);
int onlyNumber(int* num);