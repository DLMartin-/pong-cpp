#include "game_main.h"

#include <chrono>
#include <iostream>
#include <variant>

#include <SDL2/SDL.h>

#include "ball.h"

namespace {
using Ticks = std::chrono::duration<double, std::milli>;
Ticks const MAX_TICKS(1000. / 60.);

struct Pong {
  Ball ball{};
};
using ScreenVariant = std::variant<Pong>;
ScreenVariant screen{};

SDL_Event event{};
bool isRunning{true};
unsigned int frameCounter{0};
unsigned int secondsCounter{0};

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
  pong.ball.bounds.x += static_cast<int>(dt.count() * pong.ball.speed *
                                         static_cast<int>(pong.ball.direction));
  if (pong.ball.bounds.x > 750)
    pong.ball.direction = Ball::Direction::LEFT;

  if (pong.ball.bounds.x < (0 - pong.ball.bounds.w))
    pong.ball.direction = Ball::Direction::RIGHT;
}

void draw(SDL_Renderer *const renderer) {
  std::visit([renderer](auto &&screen_) { draw(screen_, renderer); }, screen);
}

void draw(Ball const &ball, SDL_Renderer *const renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(renderer, &ball.bounds);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void draw(Pong const &pong, SDL_Renderer *const renderer) {
  draw(pong.ball, renderer);
}

