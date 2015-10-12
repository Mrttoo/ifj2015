/* Test file for lexical analysis - draft */
//#include <cstdio>
//#include <cstdlib>
//#include <string>
//#include "error.h"
// Test

/* Multiline
 * comment
 / test ** /*
*/

string escaped = "Test1\"test2\"test3\"\t\n\\\"\\";
int testint = 200;
double testdouble = 300.01;
double testexpdouble = 400E12;
double testexpdouble2 = 500E+15;
double testexpdouble3 = 600.20E20;
double testexpdouble4 = 700.20E+30;
double testexpdouble5 = 800.20E-30;
double testexpdouble6 = 900E-30;

double leadingzeroes = 0000015;
double leadingzeroes2 = 15.0E00014;
double leadingzeroes3 = 14.000E-00013;
double leadingzeroes4 = 00020.E+00000004;
double leadingzeroes5 = 14.E0;
double leadingzeroes6 = 15.E0000;
double leadingzeroes7 = 0.1;
double leadingzeroes8 = 0000.001;
double leadingzeroes9 = 0000.000E0001;

double questionable  = 900.E100;

double expression1 = 20.0 + 300 - 1.2 * 5E+9 / 2;
int expression2 = (testint <= 2);
int expression3 = (testint >= 2);
int expression4 = (testint < 2);
int expression5 = (testint > 2);
int expression6 = (testint == 2);
int expression7 = (testint != 2);

/* PRogram 3: Prace s retezci a vestavenymi funkcemi */
int main()
{
    string str1;
    { // vnoreny blok s lokalni promennou str2 a pristupem k str1
        int x;
        str1 = "Toto je nejaky text";
        string str2;
        str2 = concat(str1, ", ktery jeste trochu obohatime");
        x = find(str2, "text");
        cout << "Pozice retezce \"text\" v retezci str2: "
             << x << "\n";
        cout << "Zadejte nejakou posloupnost vsech malych pismen a-h, "
             << "pricemz se pismena nesmeji v posloupnosti opakovat: ";
    }
    cin >> str1;
    str1 = sort(str1);
    if (str1 != "abcdefgh")
    {
        for (auto s = str1; s != "abcdefgh"; s = s)
        {
            cout << "Spatne zadana posloupnost, zkuste znovu: ";
            cin >> str1;
            s = sort(str1);
        }
    }
    else {}
    return 0;
}
