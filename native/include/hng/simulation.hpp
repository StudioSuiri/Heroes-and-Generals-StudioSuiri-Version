#pragma once
#include "hng/protocol.hpp"
#include <cstdint>
#include <vector>

namespace hng {
class Simulation {
public:
  std::uint32_t add_player();
  void remove_player(std::uint32_t player_id);
  bool set_input(std::uint32_t player_id, const ClientInput& input);
  void tick(float delta_seconds);
  [[nodiscard]] WorldSnapshot snapshot() const;
  [[nodiscard]] std::size_t player_count() const noexcept;
private:
  struct Player {
    PlayerSnapshot state;
    ClientInput input;
    float x{};
    float y{};
    float respawn_seconds{};
    float fire_cooldown_seconds{};
    float aim_x{1.0F};
    float aim_y{};
  };
  void update_player(Player& player, float delta_seconds);
  void resolve_shot(Player& shooter);
  void update_capture(float delta_seconds);
  Player* find_player(std::uint32_t player_id);
  const Player* find_player(std::uint32_t player_id) const;
  std::vector<Player> players_;
  std::uint32_t next_player_id_{1};
  std::uint32_t server_tick_{};
  float capture_progress_{};
};
} // namespace hng
