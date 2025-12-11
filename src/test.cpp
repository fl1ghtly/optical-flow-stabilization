#include <iostream>
#include "test.h"

bool sayHello(int x) {
    std::cout << "Hello" << std::endl;

    return x % 2 == 0 ? true : false;
}