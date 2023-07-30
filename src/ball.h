#ifndef PONG_BALL_H_
#define PONG_BALL_H_

#include <SDL2/SDL_rect.h>

struct Ball {
  SDL_Rect bounds{10, 10, 10, 10};
  float speed = 0.2;
  enum struct Direction { LEFT = -1, RIGHT = 1 };
  Direction direction = Direction::RIGHT;
};

#endif

