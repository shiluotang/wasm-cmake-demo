#include <cstdlib>
#include <iostream>

#include <QString>
#include <string>
#include <type_traits>

// #define USE_CXX17

#ifdef USE_CXX17
#   include <tuple>
#endif

namespace {

#ifdef USE_CXX17
// 1. 轉換器：將 Qt/STL 類型轉為 C 風格指標
template <typename T>
decltype(auto) toCArg(T&& arg) {
    if constexpr (std::is_same_v<typename std::decay<T>::type, QString>) {
        // QString -> const char*
        return arg.toLocal8Bit();
    } else if constexpr (std::is_same_v<typename std::decay<T>::type, std::string>) {
        // std::string -> const char*
        return arg.c_str();
    } else {
        // 基本類型 (int, double, char*) 直接返回
        return std::forward<T>(arg);
    }
}

template <typename... Args>
QString qSprintf(const char* format, Args&&... args) {
    auto tt = std::make_tuple(toCArg(std::forward<Args>(args))...);
    return std::apply([&format](auto &&...processed) {
        auto getPtr = [](auto&& item) {
            using IT = std::decay_t<decltype(item)>;
            if constexpr (std::is_same_v<IT, QByteArray>) return item.constData();
            else if constexpr (std::is_same_v<IT, std::string>) return item.c_str();
            else return item;
        };
        return QString::asprintf(format, getPtr(processed)...);
        }, tt);
    // 將所有參數經由 toCArg 轉換後，傳給原生 asprintf
    // return QString::asprintf(format, toCArg<Args>(std::forward<Args>(args))...);
}
#else
template <typename T>
typename std::enable_if<
    !std::is_same<typename std::decay<T>::type, QString>::value,
    // && !std::is_same<typename std::decay<T>::type, std::string>::value,
    T&&
>::type&& toSafeArg(T &&arg) { return std::forward<T>(arg); }
QByteArray toSafeArg(QString const &s) { return s.toLocal8Bit(); }
// QByteArray toSafeArg(QString &&s) { return s.toLocal8Bit(); }
// std::string toSafeArg(std::string const &s) { return s; }
// std::string toSafeArg(std::string &&s) { return std::move(s); }
template <typename T>
typename std::enable_if<
    !std::is_same<typename std::decay<T>::type, QString>::value &&
    !std::is_same<typename std::decay<T>::type, std::string>::value,
    T&&
>::type&&
getFinalArg(T&& arg) { return std::forward<T>(arg); }
char const* getFinalArg(QByteArray const &ba) { return ba.constData(); }
char const* getFinalArg(QByteArray &&ba) { return ba.constData(); }
char const* getFinalArg(std::string const &s) { return s.c_str(); }
// char const* getFinalArg(std::string &&s) { return s.c_str(); }
template <typename... Args>
QString qSprintf(const char* format, Args&& ...args) {
    return QString::asprintf(format, getFinalArg(toSafeArg(std::forward<Args>(args)))...);
}
#endif

std::ostream& operator<<(std::ostream &os, QString const &s) {
    os << s.toLocal8Bit().constData();
    return os;
}

} // anonymous

void test_io_test1() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void test_io_test2() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    QString kk = "Mr White";
    std::string pp = "Mr Black";
    QString s = qSprintf("Hello, %s", kk);
    QString s2 = QString::asprintf("Hello, %s", kk.toStdString().c_str());
    QString s3 = qSprintf("Hello, %s", "WTF");
    QString s4 = qSprintf("Hello, %s", QString("DAMIT"));
    QString s5 = qSprintf("Hello, %s", pp);
    QString s6 = qSprintf("Hello, %s", std::string("Mr Black"));
    std::cout << "kk = " << kk << std::endl;
    std::cout << "s = " << s << std::endl;
    std::cout << "s2 = " << s2 << std::endl;
    std::cout << "s3 = " << s3 << std::endl;
    std::cout << "s4 = " << s4 << std::endl;
    std::cout << "s5 = " << s5 << std::endl;
    std::cout << "s6 = " << s6 << std::endl;
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
