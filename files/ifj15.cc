#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

int length(string s)
{
	return s.length();
}

string concat(string s1, string s2)
{
	return s1 + s2;
}

string substr(string s, int pos, int n)
{
	return s.substr(pos, n);
}

int find(string s1, string s2)
{
	return s1.find(s2);
}

string sort(string s)
{
	std::sort(s.begin(), s.end());
	return s;
}

/* Zde bude nasledovat program jazyka IFJ15 */
