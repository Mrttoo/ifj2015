int main()
{
    int a = 3 * 2 - 1;
    double b = 3 * 5; 
    string s = "Teststring";
    auto ii = 5;

    a = a * 9;

    cout << a << "\n";
    cout << b << "\n";
    cout << s << "\n";

    if(a > 0) {
        cout << "It works\n";
    } else {
        cout << "It doesn't work\n";
    }

    for(int i = 0; i > 2; i = i + 1) {
        cout << "For: " << i << "\n";
    }

    return 0;

}
