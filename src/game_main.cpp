#include "game_main.h"

#include <chrono>
#include <iostream>
#include <variant>

#include <SDL2/SDL.h>

namespace {
using Ticks = std::chrono::duration<double, std::milli>;
Ticks const MAX_TICKS(1000. / 60.);

struct Pong {
  unsigned int counter{0};
  Ticks elapsedTime{0.};
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

// void draw();
// void draw(Pong& pong);

[[nodiscard]] GameResult
game_main([[maybe_unused]] ArgsVec const &args) noexcept {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    return GameResult::Error_Initialization;

  auto *const window = SDL_CreateWindow("Title", SDL_WINDOWPOS_UNDEFINED,
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
  pong.elapsedTime += dt;
  if (pong.elapsedTime >= Ticks(1000.)) {
    pong.elapsedTime -= Ticks(1000.);
    pong.counter += 1;

    std::cout << "(NEW) Seconds elapsed: " << pong.counter << "\n";
    if (pong.counter == 10)
      isRunning = false;
  }
}
