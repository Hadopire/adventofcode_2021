#include <fstream>
#include <iostream>

int main()
{
    std::fstream file("d1.txt");
    int prev;
    int current;
    int prevTriplet;
    int currentTriplet = 0;
    file >> prev;
    prevTriplet = prev;

    int a = 0;
    int b = 0;
    for (int i = 0; file >> current; ++i) {
        if (current > prev) {
            ++a;
        }

        if (i > 1) {
            currentTriplet += current;
            if (currentTriplet > prevTriplet) {
                ++b;
            }

            prevTriplet = currentTriplet;
            currentTriplet = prev + current;
        } else {
            prevTriplet += current;
            currentTriplet += current;
        }
        
        prev = current;
    }

    std::cout << "first gaulois: " << a << std::endl;
    std::cout << "second gaulois: " << b << std::endl;
    return 0;
}