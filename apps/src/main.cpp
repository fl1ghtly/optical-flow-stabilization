#include <iostream>
#include "test.h"

int main() {
    bool res = sayHello(2);
    std::cout << res << std::endl;
    res = sayHello(3);
    std::cout << res << std::endl;
    std::cin.get();

    return 0;
}