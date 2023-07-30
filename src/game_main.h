#ifndef GAME_MAIN_H_
#define GAME_MAIN_H_

#include <string_view>
#include <vector>

enum class GameResult {
  Ok,
  Error_Initialization,
  Error_Window_Creation,
  Error_Renderer_Creation,
};

using ArgsVec = std::vector<std::string_view>;

[[nodiscard]] GameResult game_main([[maybe_unused]] ArgsVec const &args) noexcept;

#endif

