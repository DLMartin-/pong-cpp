#include <string_view>
#include <vector>

#include "game_main.h"

int main(int const argc, char const *const *const argv) {
  ArgsVec const args(
      argv, std::next(argv, static_cast<std::ptrdiff_t>(argc)));

  return static_cast<int>(game_main(args));
}

