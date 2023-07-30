#ifndef PONG_BALL_H_
#define PONG_BALL_H_

#include <SDL2/SDL_rect.h>

struct Ball {
  //Middle of the screen x = (750 / 2) - 5
  //Middle of the screen y = (300 / 2) - 5
  SDL_Rect bounds{(750 / 2) - 5, (300 / 2) - 5, 10, 10};
  float speed = 0.2;
  enum struct Direction { LEFT = -1, RIGHT = 1 };
  Direction direction = Direction::RIGHT;
};

#endif
