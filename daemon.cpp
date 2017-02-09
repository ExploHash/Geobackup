// my first program in C++
#include <iostream>
#include <fstream>
using namespace std;
const string Programpath = "/usr/geobackup/";

int main()
{
  cout << "Hello World!";
}

bool isInstalled(){
if (std::ifstream(name))
{
     return true;
}
std::ofstream file(name);
if (!file)
{
     return false;
}
}