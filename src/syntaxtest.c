int length(string s);
string substr(string s, int i, int n);
string concat(string s1, string s2);
int find(string s, string search);
string sort(string s);

int function();
int function2();

int main() { 
    int x; 
    string y;

    cin >> x >> y;
    cout << "You entered: " << x << "AND" << y << "END OF INPUT";

    if(x) {
        cout << "IF\n";
    } else {
        cout << "ELSE\n";
    }

    cout << "AFTER IF";

    for(int i = x; x; x = x) {
        cout << "FOR";
    }

    x = function();

    return x; 
}

int function() {
    int x;
    cout << "FUNCTION";

    x = function2();

    return x;
}

int function2() {
    int x;
    cout << "FUNCITON 2";

    return x;
}
//string test(int p)
//{
//    int var1;
//    string sdfasd223_234222;
//    double _num;
//    auto testiceq = var1;
//
//    var1 = _num;
//
//    if(var1) {
//         int onlyinif;
//        _num = test;
//    } else {
//         int onlyinelse;
//        _num = testiceq;
//    }
//
//    for(int i = _num; _num; i = _num) {
//        if(_num) {
//            int intforif;
//            _num = intforif;
//            return _num;
//        } else {
//            testiceq = i;
//        }
//    }
//
//    return var1;
//}
//
//double test2(int a, int b, string c)
//{ 
//    return y;
//}
//
