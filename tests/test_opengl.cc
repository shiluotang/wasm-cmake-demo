#include <cstdlib>
#include <ctime>
#include <cstdio>

#include <sstream>
#include <iostream>
#include <vector>
#include <string>

#include <SDL/SDL.h>
#include <GL/glfw.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace {

std::string timestamp_str(std::time_t t = std::time(NULL)) {
    std::vector<char> buffer(50);
    struct tm *tm_ptr = std::localtime(&t);
    strftime(&buffer[0], buffer.size(), "%Y-%m-%d %H:%M:%S", tm_ptr);
    return &buffer[0];
}

} // namespace anonymous

#ifndef LOGX
#   define LOGX(x) \
    do { \
        std::string ts = timestamp_str(); \
        std::ostringstream __logger_oss; \
        __logger_oss << ts << " " \
            << __FILE__ << ":" << __LINE__ \
            << " " << x << std::endl; \
        std::cout << __logger_oss.str() << std::flush; \
    } while (false)
#   define LOGD(x) LOGX(x)
#endif

namespace {

class version {
public:
    version()
        : _M_major(0)
        , _M_minor(0)
        , _M_revision(0)
    {
    }

    void print(std::ostream &out) const {
        out << "{major = " << _M_major
            << ", minor = " << _M_minor
            << ", revision = " << _M_revision
            << "}";
    }

    int get_major() const { return _M_major; }
    int get_minor() const { return _M_minor; }
    int get_revision() const { return _M_revision; }

    void set_major(int value) { _M_major = value; }
    void set_minor(int value) { _M_minor = value; }
    void set_revision(int value) { _M_revision = value; }

    static
    version get_glfw_version() {
        version v;
        glfwGetVersion(&v._M_major, &v._M_minor, &v._M_revision);
        return v;
    }

    static
    version get_gl_version() {
        version v;
        glfwGetGLVersion(&v._M_major, &v._M_minor, &v._M_revision);
        return v;
    }

protected:
private:
    int _M_major;
    int _M_minor;
    int _M_revision;
};

std::ostream& operator<<(std::ostream &os, version const &obj) {
    obj.print(os);
    return os;
}

class glfw_env {
    public:
        glfw_env() :_M_initialized(false) {
            init();
        }

        ~glfw_env() {
            free();
        }
    protected:
        void init() {
            int rc = ::glfwInit();
            LOGD("glfwInit() = " << rc);
            if (!!rc)
                _M_initialized = true;
            LOGD("GLFW = " << version::get_glfw_version());
            LOGD("OpenGL = " << version::get_gl_version());
        }

        void free() {
            if (_M_initialized) {
                glfwTerminate();
                _M_initialized = false;
            }
        }
    private:
        bool _M_initialized;

};

} // namespace anonymous


void draw_frame() {
    static float rotation = 0;
    int width, height;
    glfwGetWindowSize(&width, &height);
    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(rotation, 0.0f, 0.0f, 1.0f); // 繞 Z 軸旋轉

    // 4. 繪製三角形
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(-0.5f, -0.4f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(0.5f, -0.4f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(0.0f, 0.6f);
    glEnd();

    // 更新旋轉角度
    rotation += 1.0f;
    glfwSwapBuffers();
}

void test_opengl() {
    LOGD(__PRETTY_FUNCTION__);
    float rotation = 0.0f;

    int rc = 0;
    rc = glfwInit();
    LOGD("glfwInit() = " << rc);
    if (!rc)
        throw std::runtime_error("glfwInit");
    // 2. 開啟視窗 (寬, 高, R, G, B, Alpha, Depth, Stencil, 模式)
    rc = glfwOpenWindow(640, 480, 8, 8, 8, 8, 24, 0, GLFW_WINDOW);
    LOGD("glfwOpenWindow(640, 480, 8, 8, 8, 8, 24, 0, GLFW_WINDOW) = " << rc);
    if (!rc) {
        glfwTerminate();
        throw std::runtime_error("glfwOpenWindow");
    }
    glfwSetWindowTitle("GLFW 2 Rotating Triangle");
    LOGD("glfwSetWindowTitle(\"GLFW 2 Rotating Triangle\")");
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(draw_frame, 0, EM_FALSE);
#else
    while (glfwGetWindowParam(GLFW_OPENED)) {
        // 設定視口與投影 (簡單起見使用固定管線)
        draw_frame();
    }
    // 6. 清理並退出
    glfwTerminate();
    LOGD("glfwTerminate()");
#endif
}

#ifdef __EMSCRIPTEN__
int main() {
#else
int main(int argc, char* argv[]) {
#endif
    test_opengl();
    return EXIT_SUCCESS;
}
