#include <cstdlib>
#include <iostream>


void test_io_test1() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void test_io_test2() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

#ifdef __EMSCRIPTEN__
int main() try {
#else
int main(int argc, char* argv[]) try {
#endif
    test_io_test1();
    test_io_test2();
    return EXIT_SUCCESS;
} catch (std::exception const &e) {
    std::cerr << "[c++ exception] " << e.what() << std::endl;
    return EXIT_FAILURE;
} catch (...) {
    std::cerr << "[c++ exception] " << "<UNKNOWN CAUSE>" << std::endl;
    return EXIT_FAILURE;
}
