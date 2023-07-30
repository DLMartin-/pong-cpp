#include "game_main.h"

#include <chrono>
#include <iostream>

#include <SDL2/SDL.h>

namespace {
using Ticks = std::chrono::duration<double, std::milli>;
Ticks const MAX_TICKS(1000. / 60.);

SDL_Event event{};
bool isRunning{true};
unsigned int frameCounter{0};
unsigned int secondsCounter{0};

} // namespace

[[nodiscard]] GameResult game_main([[maybe_unused]] ArgsVec const &args) noexcept {
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

      frameCounter++;
      if (frameCounter == 60) {
        frameCounter = 0;
        secondsCounter++;
        if (secondsCounter == 10)
          isRunning = false;
        std::cout << "Seconds elapsed: " << secondsCounter << "\n";
      }
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

