/* Test file for lexical analysis - draft */
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
// Test

char *escaped = "Test1\"test2\"test3\"\t\n\\\"\\";
int testint = 200;
double testdouble = 300.01;
double testexpdouble = 400E12;
double testexpdouble2 = 500E+15;
double testexpdouble3 = 600.20E20;
double testexpdouble4 = 700.20E+30;
double testexpdouble5 = 800.20E-30;
double testexpdouble6 = 900E-30;

double questionable  = 900.E100;

double invalid2 = 20.0 / 30.E2;;

double expression1 = 20.0 + 300 - 1.2 * 5E+9 / 2;
int expression2 = (testint <= 2);
int expression3 = (testint >= 2);
int expression4 = (testint < 2);
int expression5 = (testint > 2);
int expression6 = (testint == 2);
int expression7 = (testint != 2);

int main(int argc, char *argv[])
{
    int ec = IFJ_OK;

    if(argc >= 2) {
        printf("Source file: %s\n", argv[1]);
    } else {
        printf("No source file specified.\n");
        ec = IFJ_INTERNAL_ERR;
    }

   return ec;
}
