#include "game_main.h"

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_rect.h>
#include <chrono>
#include <iostream>
#include <variant>

#include <SDL2/SDL.h>

#include "ball.h"
#include "paddle.h"

namespace {
using Ticks = std::chrono::duration<double, std::milli>;
Ticks const MAX_TICKS(1000. / 60.);

struct Pong {
  enum struct State { SERVING, PLAYING };
  State state{State::SERVING};
  unsigned int counter{0};
  Ball ball{};
  Paddle leftPaddle{20, 125, 15, 50};
  Paddle rightPaddle{720, 125, 15, 50};
  unsigned char leftPaddleScore{0};
  unsigned char rightPaddleScore{0};
};
using ScreenVariant = std::variant<Pong>;
ScreenVariant screen{};

SDL_Event event{};
bool isRunning{true};
unsigned int frameCounter{0};
unsigned int secondsCounter{0};
Uint8 const *keyboardState = SDL_GetKeyboardState(nullptr);

} // namespace

void update(Ticks const &dt);
void update(Pong &pong, Ticks const &dt);

void draw(SDL_Renderer *const renderer);
void draw(Ball const &ball, SDL_Renderer *const renderer);
void draw(Pong const &pong, SDL_Renderer *const renderer);

[[nodiscard]] GameResult
game_main([[maybe_unused]] ArgsVec const &args) noexcept {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    return GameResult::Error_Initialization;

  auto *const window = SDL_CreateWindow("Pong!!!!", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, 750, 300, 0);
  if (!window)
    return GameResult::Error_Window_Creation;

  auto *const renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer)
    return GameResult::Error_Renderer_Creation;

  auto currentTime = std::chrono::steady_clock::now();
  Ticks accumulator(0.);
  while (isRunning) {

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        isRunning = false;
    }

    auto const newTime = std::chrono::steady_clock::now();
    auto const frameTime =
        std::chrono::duration_cast<Ticks>(newTime - currentTime);
    currentTime = newTime;
    accumulator += frameTime;

    while (accumulator >= MAX_TICKS) {
      if (!isRunning)
        break;

      update(MAX_TICKS);
      accumulator -= MAX_TICKS;
    }

    SDL_RenderClear(renderer);

    /* Draw Here */
    draw(renderer);
    /* End Draw */
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return GameResult::Ok;
}

void update(Ticks const &dt) {
  std::visit([dt](auto &&screen_) { update(screen_, dt); }, screen);
}

void update(Pong &pong, Ticks const &dt) {
  static Ticks accumulator{0.};
  switch (pong.state) {
  case Pong::State::SERVING:
    accumulator += dt;
    if (accumulator >= Ticks(1000.)) {
      accumulator = accumulator - Ticks(1000.);
      if (pong.counter < 3) {
        // We need to flush, otherwise the text won't appear in the console
        // until after the state has changed to PLAYING
        std::cout << (3 - pong.counter) << "..." << std::flush;
      }
      pong.counter++;
    }

    if (pong.counter > 3) {
      std::cout << "START!!!\n";
      pong.state = Pong::State::PLAYING;
      accumulator = Ticks(0.);
      pong.counter = 0;
    }
    break;

  case Pong::State::PLAYING:
    bool didScore = false;
    pong.ball.bounds.x += static_cast<int>(
        dt.count() * pong.ball.speed * static_cast<int>(pong.ball.direction));

    pong.ball.bounds.y += static_cast<int>(dt.count() * pong.ball.ySpeed);

    if (keyboardState[SDL_SCANCODE_A]) {
      pong.leftPaddle.bounds.y -=
          static_cast<int>(dt.count() * pong.leftPaddle.speed);
      if (pong.leftPaddle.bounds.y < 0)
        pong.leftPaddle.bounds.y = 0;
    }

    if (keyboardState[SDL_SCANCODE_Z]) {
      pong.leftPaddle.bounds.y +=
          static_cast<int>(dt.count() * pong.leftPaddle.speed);
      if (pong.leftPaddle.bounds.y + pong.leftPaddle.bounds.h > 300)
        pong.leftPaddle.bounds.y = 300 - pong.leftPaddle.bounds.h;
    }

    if (keyboardState[SDL_SCANCODE_K]) {

      pong.rightPaddle.bounds.y -=
          static_cast<int>(dt.count() * pong.rightPaddle.speed);
      if (pong.rightPaddle.bounds.y < 0)
        pong.rightPaddle.bounds.y = 0;
    }

    if (keyboardState[SDL_SCANCODE_M]) {
      pong.rightPaddle.bounds.y +=
          static_cast<int>(dt.count() * pong.leftPaddle.speed);
      if (pong.rightPaddle.bounds.y + pong.rightPaddle.bounds.h > 300)
        pong.rightPaddle.bounds.y = 300 - pong.rightPaddle.bounds.h;
    }

    // Collision Checking against left paddle and ball
    if (SDL_HasIntersection(&pong.ball.bounds, &pong.leftPaddle.bounds) ==
        SDL_TRUE) {
      if (pong.ball.bounds.y >
          (pong.leftPaddle.bounds.y + pong.leftPaddle.bounds.h - 15)) {
        pong.ball.ySpeed = 0.4f;
      }

      if ((pong.ball.bounds.y + pong.ball.bounds.h) <
          pong.leftPaddle.bounds.y + 15) {
        pong.ball.ySpeed = -0.4f;
      }

      if (pong.ball.bounds.x >
          (pong.leftPaddle.bounds.x + pong.leftPaddle.bounds.w) - 5) {
        pong.ball.bounds.x =
            pong.leftPaddle.bounds.x + pong.leftPaddle.bounds.w;
        pong.ball.direction = Ball::Direction::RIGHT;
      }
    }

    // Collision checking against right paddle and ball
    if (SDL_HasIntersection(&pong.ball.bounds, &pong.rightPaddle.bounds) ==
        SDL_TRUE) {
      if (pong.ball.bounds.y >
          (pong.rightPaddle.bounds.y + pong.rightPaddle.bounds.h - 15)) {
        pong.ball.ySpeed = 0.4f;
      }

      if ((pong.ball.bounds.y + pong.ball.bounds.h) <
          pong.rightPaddle.bounds.y + 15) {
        pong.ball.ySpeed = -0.4f;
      }

      if ((pong.ball.bounds.x + pong.ball.bounds.w) <
          pong.rightPaddle.bounds.x + 5) {
        pong.ball.bounds.x =
            pong.rightPaddle.bounds.x - pong.rightPaddle.bounds.w;
        pong.ball.direction = Ball::Direction::LEFT;
      }
    }

    // Collision checking against top and bottom of screen
    if (pong.ball.bounds.y < 0) {
      pong.ball.bounds.y = 0;
      pong.ball.ySpeed = -pong.ball.ySpeed;
    }

    if ((pong.ball.bounds.y + pong.ball.bounds.h) > 300) {
      pong.ball.bounds.y = 300 - pong.ball.bounds.h;
      pong.ball.ySpeed = -pong.ball.ySpeed;
    }

    if (pong.ball.bounds.x > 750) {
      didScore = true;
      pong.leftPaddleScore++;
    }

    if (pong.ball.bounds.x < (0 - pong.ball.bounds.w)) {
      didScore = true;
      pong.rightPaddleScore++;
    }

    if (didScore) {
      if (pong.ball.direction == Ball::Direction::LEFT)
        pong.ball.direction = Ball::Direction::RIGHT;
      else
        pong.ball.direction = Ball::Direction::LEFT;

      pong.ball.bounds.x = (750 / 2) - (pong.ball.bounds.w / 2);
      pong.ball.bounds.y = (300 / 2) - (pong.ball.bounds.h / 2);
      pong.ball.ySpeed = 0.;
      pong.state = Pong::State::SERVING;

      std::cout << "-----Score-----\n";
      std::cout << "Left Paddle: "
                << static_cast<unsigned int>(pong.leftPaddleScore) << "\n";
      std::cout << "Right Paddle: "
                << static_cast<unsigned int>(pong.rightPaddleScore) << "\n";
      std::cout << std::endl;
    }
    break;
  }
}

void draw(SDL_Renderer *const renderer) {
  std::visit([renderer](auto &&screen_) { draw(screen_, renderer); }, screen);
}

void draw(Ball const &ball, SDL_Renderer *const renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(renderer, &ball.bounds);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void draw(Paddle const &paddle, SDL_Renderer *const renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
  SDL_RenderFillRect(renderer, &paddle.bounds);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void draw(Pong const &pong, SDL_Renderer *const renderer) {
  draw(pong.ball, renderer);
  draw(pong.leftPaddle, renderer);
  draw(pong.rightPaddle, renderer);
}
