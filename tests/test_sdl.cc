#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>

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

class AppState {
public:
    AppState()
        : _M_surface(0)
        , _M_event()
        , _M_quit(false)
    {
        std::memset(&_M_event, 0, sizeof(_M_event));
        Uint32 rc = SDL_Init(SDL_INIT_VIDEO);
        LOGD("SDL_Init(SDL_INIT_VIDEO) = " << rc);
        int width = 256;
        int height = 256;
        int bpp = 32;
        Uint32 flags = SDL_SWSURFACE | SDL_DOUBLEBUF;
        SDL_Surface *screen = SDL_SetVideoMode(width, height, bpp, flags);
        LOGD("SDL_SetVideoMode(256, 256, 32, SDL_SWSURFACE) = " << screen);
        _M_surface = screen;
    }

    ~AppState() {
        LOGD(__PRETTY_FUNCTION__);
        SDL_Quit();
        LOGD("SDL_Quit()");
    }

    void setQuit(bool value) {
        _M_quit = value;
    }

    void checkEvent() {
        if (!SDL_PollEvent(&_M_event))
            return;
        switch (_M_event.type) {
            case SDL_QUIT:
                setQuit(true);
                break;
            case SDL_KEYDOWN:
                if (_M_event.key.keysym.sym == SDLK_ESCAPE)
                    setQuit(true);
                break;
            default: break;
        }
    }

    bool shouldQuit() {
        if (_M_quit)
            return true;
        checkEvent();
        return _M_quit;
    }

    void beginScene() {
        if (SDL_MUSTLOCK(_M_surface)) {
            int rc = SDL_LockSurface(_M_surface);
            LOGD("SDL_LockSurface(screen = " << _M_surface << ") = " << rc);
        }
    }

    void endScene() {
        if (SDL_MUSTLOCK(_M_surface)) {
            SDL_UnlockSurface(_M_surface);
            LOGD("SDL_UnlockSurface(screen = " << _M_surface << ")");
        }
        // SDL_Flip is SDL1.2 API, where SDL_RenderPresent is used in SDL2,
        // emscripten use SDL1 by default. So "-s USE_SDL=2" link option is
        // preventing the actual rendering
        int rc = SDL_Flip(_M_surface);
        LOGD("SDL_Flip(screen = " << _M_surface << ") = " << rc);
    }

    SDL_Surface* getSurface() const {
        return _M_surface;
    }
protected:
private:
    SDL_Surface *_M_surface;
    SDL_Event _M_event;
    bool _M_quit;
};

void draw_frame(void *app) {
    AppState *state = static_cast<AppState*>(app);
    if (state->shouldQuit()) {
#ifdef __EMSCRIPTEN__
        LOGD(">>> emscripten_cancel_main_loop()");
        emscripten_cancel_main_loop();
        LOGD("<<< emscripten_cancel_main_loop()");
#endif
        return;
    }
    state->beginScene();
    SDL_Surface *screen = state->getSurface();
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
    state->endScene();
}

void test_sdl() {
    LOGD("hello, xxx world!");
    AppState state;

#ifdef TEST_SDL_LOCK_OPTS
    EM_ASM("SDL.defaults.copyOnLock = false; ");
    EM_ASM("SDL.defaults.discardOnLock = true;");
    EM_ASM("SDL.defaults.opaqueFrontBuffer = false;");
#endif
    LOGD("you should see a smoothly-colored square - no sharp lines but the square borders!");
    LOGD("and here is some text that should be HTML-friendly: amp: |&| double-quote: |\"| quote: |'| less-than, greater-than, html-like tags: |<cheez></cheez>|");
#ifdef __EMSCRIPTEN__
    LOGD(">>> emscripten_set_main_loop_arg(draw_frame, &state, -1, EM_TRUE)");
    emscripten_set_main_loop_arg(draw_frame, &state, -1, EM_TRUE);
    LOGD("<<< emscripten_set_main_loop_arg(draw_frame, &state, -1, EM_TRUE)");
    // draw_frame(&state);
#else
    while (!state.shouldQuit()) {
        draw_frame(&state);
        SDL_Delay(1);
    }
#endif
}

#ifdef __EMSCRIPTEN__
int main() {
#else
int main(int argc, char* argv[]) {
#endif
    test_sdl();
#ifdef __EMSCRIPTEN__
    emscripten_sleep(100);
    LOGD(">>> emscripten_force_exit(EXIT_SUCCESS)");
    emscripten_force_exit(EXIT_SUCCESS);
    LOGD("<<< emscripten_force_exit(EXIT_SUCCESS)");
#endif
    return EXIT_SUCCESS;
}
