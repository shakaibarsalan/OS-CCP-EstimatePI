
#include <iostream>
#include <cstdlib>
#include <cmath>
using namespace std;

long double maclaurin_polynomial_for_PI(unsigned int n)
{
    return pow(-1, n) / (2 * n + 1);
}

double compute_pi(unsigned int n)
{
    double pi = 0;
    for (unsigned int i = 0; i < n; i++)
    {
        pi += maclaurin_polynomial_for_PI(i);
    }
    return pi * 4;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Invalid argument count\n";
        return 1;
    }

    unsigned int n = static_cast<unsigned int>(atoi(argv[1]));
    double PI = compute_pi(n);

    cout << "PI: " << PI << endl;

    return 0;
}
