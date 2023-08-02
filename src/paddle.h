#ifndef PONG_PADDLE_H_
#define PONG_PADDLE_H_

#include <SDL2/SDL_rect.h>

struct Paddle {
  Paddle(int x, int y, int w, int h) : bounds(x, y, w, h) {}
  SDL_Rect bounds{0, 0, 0, 0};
  float speed = 0.2f;
};

#endif
