#include <cstdlib>
#include <ctime>
#include <cmath>

#include <sstream>
#include <iostream>
#include <vector>
#include <string>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
// 假設環境已配置好 GLES2 轉接
#ifdef __EMSCRIPTEN__
#include <GL/glfw.h>
#else
#include <GLFW/glfw3.h>
#endif

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


// --- Shader 程式碼 ---
const char* vertex_shader_src =
    "attribute vec2 position;    \n"
    "attribute vec3 color;       \n"
    "varying vec3 vColor;        \n"
    "uniform float angle;        \n"
    "void main() {               \n"
    "   float s = sin(angle);    \n"
    "   float c = cos(angle);    \n"
    "   mat2 rotation = mat2(c, s, -s, c); \n" // 旋轉矩陣
    "   vColor = color;          \n"
    "   gl_Position = vec4(rotation * position, 0.0, 1.0); \n"
    "} \n";

const char* fragment_shader_src =
    "precision mediump float;    \n"
    "varying vec3 vColor;        \n"
    "void main() {               \n"
    "   gl_FragColor = vec4(vColor, 1.0); \n"
    "} \n";

// 編譯 Shader 的輔助函式 (簡略版)
GLuint compile_shader(GLenum type, const char* source) {
    LOGD("compile_shader(type = " << type << ", source = " << source << ")");
    GLuint shader = glCreateShader(type);
    LOGD("glCreateShader(type = " << type << ") = " << shader);
    glShaderSource(shader, 1, &source, NULL);
    LOGD("glShaderSource(shader = " << shader
            << ", count = " << 1
            << ", string = " << &source
            << ", length = " <<  NULL << ") = " << shader);
    glCompileShader(shader);
    LOGD("glCompileShader(shader = " << shader << ")");
    LOGD("compile_shader(type = " << type << ", source = " << static_cast<void const*>(source) << ") = " << shader);
    return shader;
}


GLint angleUni = 0;
GLint posAttrib = 0;
GLint colAttrib = 0;
float vertices[] = {
     0.0f,  0.5f,  1.0f, 0.0f, 0.0f,
     -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.0f, 0.0f, 1.0f
};
float cur_angle = 0.0f;

void draw_frame() {
    // 定義三角形頂點數據 (座標 x, y + 顏色 r, g, b)
    glClear(GL_COLOR_BUFFER_BIT);
    // 傳遞數據
    glUniform1f(angleUni, cur_angle);

    GLuint vbo_vertices = 0;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
            posAttrib,
            2,
            GL_FLOAT,
            GL_FALSE,
            5 * sizeof(float),
            reinterpret_cast<void*>(0));

    GLuint vbo_colors = 0;
    glGenBuffers(1, &vbo_colors);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
            colAttrib,
            3,
            GL_FLOAT,
            GL_FALSE,
            5 * sizeof(float),
            reinterpret_cast<void*>(2 * sizeof(float)));

    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(colAttrib);
    // 繪製
    glDrawArrays(GL_TRIANGLES, 0, 3);

    cur_angle += M_PI / 180 * 0.1f;
    glfwSwapBuffers();
}

void test_glesv2() {
    int rc = 0;
    rc = glfwInit();
    if (!rc)
        throw std::runtime_error("glfwInit");
    rc = glfwOpenWindow(640, 480, 8, 8, 8, 8, 24, 0, GLFW_WINDOW);
    if (!rc)
        throw std::runtime_error("glfwOpenWindow");
    // 1. 編譯並連結 Shader Program
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    GLuint program = glCreateProgram();
    LOGD("glCreateProgram() = " << program);
    glAttachShader(program, vs);
    LOGD("glAttachShader(program = " << program << ", shader = " << vs << ")");
    glAttachShader(program, fs);
    LOGD("glAttachShader(program = " << program << ", shader = " << fs << ")");
    glLinkProgram(program);
    LOGD("glLinkProgram(program = " << program << ")");
    glUseProgram(program);
    LOGD("glUseProgram(program = " << program << ")");

    posAttrib = glGetAttribLocation(program, "position");
    LOGD("glGetAttribLocation(program = " << program << ", \"position\") = " << posAttrib);
    colAttrib = glGetAttribLocation(program, "color");
    LOGD("glGetAttribLocation(program = " << program << ", \"color\") = " << colAttrib);
    angleUni = glGetUniformLocation(program, "angle");
    LOGD("glGetAttribLocation(program = " << program << ", \"angle\") = " << angleUni);
#if __EMSCRIPTEN__
    emscripten_set_main_loop(draw_frame, 0, EM_TRUE);
#else
    while (glfwGetWindowParam(GLFW_OPENED)) {
        draw_frame();
    }
    glfwTerminate();
#endif
}

#ifdef __EMSCRIPTEN__
int main() {
#else
int main(int argc, char* argv[]) {
#endif
    test_glesv2();
    return EXIT_SUCCESS;
}
