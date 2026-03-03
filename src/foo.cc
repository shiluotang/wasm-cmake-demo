#include <iostream>

#include <foo.h>

class Greeter {
public:
    bool sayHi(std::string const &name) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::cout << "Hi, " << name << "!" << std::endl;
        return true;
    }
};

void foo() {
    Greeter greeter;
    greeter.sayHi("Mr. White");
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}
