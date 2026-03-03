#include <cstdlib>
#include <ctime>
#include <cstdio>

#include <sstream>
#include <iostream>
#include <vector>
#include <string>

#include <SDL/SDL.h>

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

void test_sdl() {
    LOGD("hello, xxx world!");
    Uint32 rc = SDL_Init(SDL_INIT_VIDEO);
    LOGD("SDL_Init(SDL_INIT_VIDEO) = " << rc);
    SDL_Surface *screen = SDL_SetVideoMode(256, 256, 32, SDL_SWSURFACE);
    LOGD("SDL_SetVideoMode(256, 256, 32, SDL_SWSURFACE) = " << screen);

#ifdef TEST_SDL_LOCK_OPTS
    EM_ASM("SDL.defaults.copyOnLock = false; SDL.defaults.discardOnLock = true; SDL.defaults.opaqueFrontBuffer = false;");
#endif

    if (SDL_MUSTLOCK(screen)) {
        rc = SDL_LockSurface(screen);
        LOGD("SDL_LockSurface(screen = " << screen << ") = " << rc);
    }
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
#ifdef TEST_SDL_LOCK_OPTS
            // Alpha behaves like in the browser, so write proper opaque pixels.
            int alpha = 255;
#else
            // To emulate native behavior with blitting to screen, alpha component is ignored. Test that it is so by outputting
            // data (and testing that it does get discarded)
            int alpha = (i+j) % 255;
#endif
            *((Uint32*)screen->pixels + i * 256 + j) = SDL_MapRGBA(screen->format, i, j, 255-i, alpha);
            // LOGD("screen.pixels[" << i << ", " << j << "] = rgba(" << i
            //         << ", " << j
            //         << ", " << (255 - i)
            //         << ", " << alpha
            //         << ")");
        }
    }
    if (SDL_MUSTLOCK(screen)) {
        SDL_UnlockSurface(screen);
        LOGD("SDL_UnlockSurface(screen = " << screen << ")");
    }
    // SDL_Flip is SDL1.2 API, where SDL_RenderPresent is used in SDL2,
    // emscripten use SDL1 by default. So "-s USE_SDL=2" link option is
    // preventing the actual rendering
    rc = SDL_Flip(screen);
    LOGD("SDL_Flip(screen = " << screen << ") = " << rc);

    printf("you should see a smoothly-colored square - no sharp lines but the square borders!\n");
    printf("and here is some text that should be HTML-friendly: amp: |&| double-quote: |\"| quote: |'| less-than, greater-than, html-like tags: |<cheez></cheez>|\nanother line.\n");

    SDL_Quit();
    LOGD("SDL_Quit()");
}

#ifdef __EMSCRIPTEN__
int main() {
#else
int main(int argc, char* argv[]) {
#endif
    test_sdl();
    return EXIT_SUCCESS;
}
