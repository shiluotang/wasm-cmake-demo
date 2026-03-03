#include <fstream>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <foo.h>

void test_ifstream() {
    std::string const filename = "/a.txt";
    std::cout << __FILE__ << std::endl;
    std::cout << "opening file " << filename << std::endl;
    std::ifstream infile(filename.c_str());
    if (!infile)
        throw std::runtime_error("failed to open file for read!");
    std::cout
        << std::string(20, '=') << filename << std::string(20, '=')
        << std::endl;
    // std::string content = std::string(
    //         std::istreambuf_iterator<char>(infile),
    //         std::istreambuf_iterator<char>());
    // std::cout << content << std::endl;
    std::cout << infile.rdbuf() << std::endl;
}

#ifdef __EMSCRIPTEN__
int main() try {
#else
int main(int argc, char* argv[]) try {
#endif
    foo();
    test_ifstream();
    return EXIT_SUCCESS;
} catch (std::exception const &e) {
    std::cerr << "[c++ exception] " << e.what() << std::endl;
    return EXIT_FAILURE;
} catch (...) {
    std::cerr << "[c++ exception] " << "<UNKNOWN CAUSE>" << std::endl;
    return EXIT_FAILURE;
}
