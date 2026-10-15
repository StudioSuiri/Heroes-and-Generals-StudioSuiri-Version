#include "hng/simulation.hpp"
#include <algorithm>
#include <cmath>

namespace hng {
namespace {
constexpr float kPlayerSpeed = 180.0F;
constexpr float kPlayerRadius = 12.0F;
constexpr float kCaptureRadius = 120.0F;
constexpr float kShotRange = 320.0F;
constexpr float kShotHalfWidth = 22.0F;
constexpr float kShotCooldown = 0.28F;
constexpr float kRespawnDelay = 2.0F;
constexpr float kCaptureRate = 18.0F;
constexpr float kNeutralizeRate = 4.0F;
constexpr std::uint8_t kShotDamage = 34;
float distance_squared(float ax, float ay, float bx, float by) {
  const auto dx = ax - bx; const auto dy = ay - by; return dx * dx + dy * dy;
}
std::int16_t quantize(float v, int max) {
  return static_cast<std::int16_t>(std::clamp(static_cast<int>(std::lround(v)), 0, max));
}
}

std::uint32_t Simulation::add_player() {
  if (players_.size() >= kMaxPlayers) return 0;
  Player p;
  p.state.id = next_player_id_++;
  p.state.faction = (p.state.id % 2U == 1U) ? 1U : 2U;
  p.state.health = 100;
  p.x = p.state.faction == 1 ? 120.0F : static_cast<float>(kWorldWidth - 120);
  p.y = std::clamp(200.0F + static_cast<float>(p.state.id % 5U) * 100.0F, 80.0F, 820.0F);
  p.state.x = quantize(p.x, kWorldWidth); p.state.y = quantize(p.y, kWorldHeight);
  p.aim_x = p.state.faction == 1 ? 1.0F : -1.0F;
  players_.push_back(p);
  return p.state.id;
}
void Simulation::remove_player(std::uint32_t id) {
  std::erase_if(players_, [id](const Player& p) { return p.state.id == id; });
}
bool Simulation::set_input(std::uint32_t id, const ClientInput& in) {
  auto* p = find_player(id);
  if (!p || p->state.health == 0) return false;
  p->input.move_x = static_cast<std::int8_t>(in.move_x > 0 ? 1 : in.move_x < 0 ? -1 : 0);
  p->input.move_y = static_cast<std::int8_t>(in.move_y > 0 ? 1 : in.move_y < 0 ? -1 : 0);
  p->input.aim_x = in.aim_x; p->input.aim_y = in.aim_y;
  p->input.buttons = static_cast<std::uint8_t>(in.buttons & kButtonFire);
  const float ax = static_cast<float>(in.aim_x), ay = static_cast<float>(in.aim_y);
  const float len = std::sqrt(ax * ax + ay * ay);
  if (len > 0.01F) { p->aim_x = ax / len; p->aim_y = ay / len; }
  return true;
}
void Simulation::tick(float dt) {
  const float d = std::clamp(dt, 0.0F, 0.1F);
  ++server_tick_;
  for (auto& p : players_) update_player(p, d);
  update_capture(d);
}
void Simulation::update_player(Player& p, float dt) {
  p.fire_cooldown_seconds = std::max(0.0F, p.fire_cooldown_seconds - dt);
  if (p.state.health == 0) {
    p.respawn_seconds = std::max(0.0F, p.respawn_seconds - dt);
    if (p.respawn_seconds <= 0.0F) {
      p.state.health = 100;
      p.x = p.state.faction == 1 ? 120.0F : static_cast<float>(kWorldWidth - 120);
      p.y = 200.0F + static_cast<float>(p.state.id % 5U) * 100.0F;
      p.input = {}; p.aim_x = p.state.faction == 1 ? 1.0F : -1.0F; p.aim_y = 0.0F;
    }
    p.state.x = quantize(p.x, kWorldWidth); p.state.y = quantize(p.y, kWorldHeight);
    return;
  }
  float mx = static_cast<float>(p.input.move_x), my = static_cast<float>(p.input.move_y);
  const float len = std::sqrt(mx * mx + my * my);
  if (len > 1.0F) { mx /= len; my /= len; }
  p.x = std::clamp(p.x + mx * kPlayerSpeed * dt, kPlayerRadius, static_cast<float>(kWorldWidth) - kPlayerRadius);
  p.y = std::clamp(p.y + my * kPlayerSpeed * dt, kPlayerRadius, static_cast<float>(kWorldHeight) - kPlayerRadius);
  p.state.x = quantize(p.x, kWorldWidth); p.state.y = quantize(p.y, kWorldHeight);
  if ((p.input.buttons & kButtonFire) && p.fire_cooldown_seconds <= 0.0F) {
    resolve_shot(p); p.fire_cooldown_seconds = kShotCooldown;
  }
}
void Simulation::resolve_shot(Player& shooter) {
  Player* hit = nullptr; float best = kShotRange;
  for (auto& target : players_) {
    if (target.state.id == shooter.state.id || target.state.faction == shooter.state.faction || target.state.health == 0) continue;
    const float dx = target.x - shooter.x, dy = target.y - shooter.y;
    const float projection = dx * shooter.aim_x + dy * shooter.aim_y;
    if (projection <= 0.0F || projection > best) continue;
    const float perpendicular = std::abs(dx * shooter.aim_y - dy * shooter.aim_x);
    if (perpendicular <= kShotHalfWidth + kPlayerRadius) { best = projection; hit = &target; }
  }
  if (!hit) return;
  const int health = static_cast<int>(hit->state.health) - kShotDamage;
  hit->state.health = static_cast<std::uint8_t>(std::max(0, health));
  if (hit->state.health == 0) {
    ++hit->state.deaths; ++shooter.state.kills; hit->respawn_seconds = kRespawnDelay; hit->input = {};
  }
}
void Simulation::update_capture(float dt) {
  constexpr float cx = static_cast<float>(kWorldWidth) / 2.0F, cy = static_cast<float>(kWorldHeight) / 2.0F;
  const float r2 = kCaptureRadius * kCaptureRadius;
  int one = 0, two = 0;
  for (const auto& p : players_) {
    if (p.state.health == 0 || distance_squared(p.x, p.y, cx, cy) > r2) continue;
    if (p.state.faction == 1) ++one; else if (p.state.faction == 2) ++two;
  }
  if (one > 0 && two == 0) capture_progress_ = std::min(100.0F, capture_progress_ + kCaptureRate * static_cast<float>(one) * dt);
  else if (two > 0 && one == 0) capture_progress_ = std::max(-100.0F, capture_progress_ - kCaptureRate * static_cast<float>(two) * dt);
  else if (one == 0 && two == 0) {
    if (capture_progress_ > 0) capture_progress_ = std::max(0.0F, capture_progress_ - kNeutralizeRate * dt);
    else if (capture_progress_ < 0) capture_progress_ = std::min(0.0F, capture_progress_ + kNeutralizeRate * dt);
  }
}
Simulation::Player* Simulation::find_player(std::uint32_t id) {
  const auto it = std::find_if(players_.begin(), players_.end(), [id](const Player& p) { return p.state.id == id; });
  return it == players_.end() ? nullptr : &*it;
}
const Simulation::Player* Simulation::find_player(std::uint32_t id) const {
  const auto it = std::find_if(players_.begin(), players_.end(), [id](const Player& p) { return p.state.id == id; });
  return it == players_.end() ? nullptr : &*it;
}
WorldSnapshot Simulation::snapshot() const {
  WorldSnapshot out; out.server_tick = server_tick_;
  out.capture_progress = static_cast<std::int16_t>(std::clamp(static_cast<int>(std::lround(capture_progress_)), -100, 100));
  out.players.reserve(players_.size());
  for (const auto& p : players_) {
    auto state = p.state;
    state.x = quantize(p.x, kWorldWidth); state.y = quantize(p.y, kWorldHeight);
    out.players.push_back(state);
  }
  return out;
}
std::size_t Simulation::player_count() const noexcept { return players_.size(); }
} // namespace hng
