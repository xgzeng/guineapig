#include <stdio.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

const int TIMER_EVENT_CODE = 1;

uint32_t TimerCallback(uint32_t interval, void* param) {
    // 发送定时器事件
    SDL_Event event;
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = TIMER_EVENT_CODE;
    userevent.data1 = NULL;
    userevent.data2 = NULL;
    event.type = SDL_USEREVENT;
    event.user = userevent;
    SDL_PushEvent(&event);
    return interval;
}

int main(int argc, char* argv[]) {
    // 初始化
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL init failed");
        return EXIT_FAILURE;
    }

    // 创建窗口
    SDL_Window *win = SDL_CreateWindow("SDLExample", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        printf("SDL_CreateWindow failed");
        return EXIT_FAILURE;
    }

    // 设置1秒的定时器
    SDL_AddTimer(1000, TimerCallback, nullptr);

    SDL_Renderer* render = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);

    while(1) {
        SDL_Event e;
        if (SDL_PollEvent(&e) == 0) {
          // 无事件需处理
          continue;
        }

        if (e.type == SDL_QUIT) {
            break;
        }

        if (e.type == SDL_KEYDOWN) { // 按键事件
            SDL_KeyCode key_code = static_cast<SDL_KeyCode>(e.key.keysym.sym);
            switch (key_code) {
            case SDLK_UP: printf("up\n"); break;
            case SDLK_DOWN: printf("down\n"); break;
            case SDLK_RIGHT: printf("right\n"); break;
            case SDLK_LEFT: printf("left\n"); break;
            default: printf("other key\n"); break;
            }
        } else if (e.type == SDL_USEREVENT && e.user.code == TIMER_EVENT_CODE) {
            // 定时器事件
            printf("timer triggered\n");
        }
        
        // 使用白色清除背景
        SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(render);

        // 使用红色绘制直线
        SDL_SetRenderDrawColor(render, 255, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(render, 0, 0, 100, 100);

        // 使用绿色填充矩形
        SDL_Rect rect = {50, 50, 10, 10}; // x, y, width, height
        SDL_SetRenderDrawColor(render, 0, 255, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(render, &rect);

        // 显示绘制内容
        SDL_RenderPresent(render);
    }

    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
