#include "hng/protocol.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>
namespace hng {
namespace {
constexpr std::uint8_t kMagic[4] = {'H', 'N', 'G', 'N'};
void append_u16(std::vector<std::uint8_t>& out, std::uint16_t v) {
  out.push_back(static_cast<std::uint8_t>(v & 0xffU)); out.push_back(static_cast<std::uint8_t>((v >> 8U) & 0xffU));
}
void append_i16(std::vector<std::uint8_t>& out, std::int16_t v) { append_u16(out, static_cast<std::uint16_t>(v)); }
void append_u32(std::vector<std::uint8_t>& out, std::uint32_t v) {
  out.push_back(static_cast<std::uint8_t>(v & 0xffU)); out.push_back(static_cast<std::uint8_t>((v >> 8U) & 0xffU));
  out.push_back(static_cast<std::uint8_t>((v >> 16U) & 0xffU)); out.push_back(static_cast<std::uint8_t>((v >> 24U) & 0xffU));
}
std::uint16_t read_u16(std::span<const std::uint8_t> b, std::size_t o) {
  return static_cast<std::uint16_t>(b[o]) | static_cast<std::uint16_t>(static_cast<std::uint16_t>(b[o + 1]) << 8U);
}
std::int16_t read_i16(std::span<const std::uint8_t> b, std::size_t o) {
  const auto v = read_u16(b, o);
  return static_cast<std::int16_t>(v < 0x8000U ? static_cast<std::int32_t>(v) : static_cast<std::int32_t>(v) - 65536);
}
std::uint32_t read_u32(std::span<const std::uint8_t> b, std::size_t o) {
  return static_cast<std::uint32_t>(b[o]) | (static_cast<std::uint32_t>(b[o + 1]) << 8U) |
    (static_cast<std::uint32_t>(b[o + 2]) << 16U) | (static_cast<std::uint32_t>(b[o + 3]) << 24U);
}
bool known_type(std::uint8_t v) { return v >= 1 && v <= 5; }
}
std::vector<std::uint8_t> encode_packet(PacketType type, std::uint32_t sequence,
    std::uint32_t client_id, std::span<const std::uint8_t> payload) {
  if (!known_type(static_cast<std::uint8_t>(type)) || payload.size() > kMaxPayloadBytes) return {};
  std::vector<std::uint8_t> b; b.reserve(kPacketHeaderBytes + payload.size());
  b.insert(b.end(), std::begin(kMagic), std::end(kMagic)); b.push_back(kProtocolVersion);
  b.push_back(static_cast<std::uint8_t>(type)); append_u16(b, static_cast<std::uint16_t>(payload.size()));
  append_u32(b, sequence); append_u32(b, client_id); b.insert(b.end(), payload.begin(), payload.end());
  return b;
}
std::optional<Packet> decode_packet(std::span<const std::uint8_t> b) {
  if (b.size() < kPacketHeaderBytes || b.size() > kMaxDatagramBytes) return std::nullopt;
  for (std::size_t i = 0; i < 4; ++i) if (b[i] != kMagic[i]) return std::nullopt;
  if (b[4] != kProtocolVersion || !known_type(b[5])) return std::nullopt;
  const auto size = read_u16(b, 6);
  if (size > kMaxPayloadBytes || b.size() != kPacketHeaderBytes + size) return std::nullopt;
  Packet p; p.type = static_cast<PacketType>(b[5]); p.sequence = read_u32(b, 8); p.client_id = read_u32(b, 12);
  p.payload.assign(b.begin() + static_cast<std::ptrdiff_t>(kPacketHeaderBytes), b.end()); return p;
}
std::vector<std::uint8_t> encode_input_payload(const ClientInput& i) {
  std::vector<std::uint8_t> p; p.reserve(7);
  p.push_back(static_cast<std::uint8_t>(i.move_x)); p.push_back(static_cast<std::uint8_t>(i.move_y));
  append_i16(p, i.aim_x); append_i16(p, i.aim_y); p.push_back(static_cast<std::uint8_t>(i.buttons & kButtonFire)); return p;
}
bool decode_input_payload(std::span<const std::uint8_t> p, ClientInput& i) {
  if (p.size() != 7) return false;
  const auto axis = [](std::uint8_t v) { const int n = v < 0x80U ? static_cast<int>(v) : static_cast<int>(v) - 256; return static_cast<std::int8_t>(std::clamp(n, -1, 1)); };
  i.move_x = axis(p[0]); i.move_y = axis(p[1]); i.aim_x = read_i16(p, 2); i.aim_y = read_i16(p, 4);
  i.buttons = static_cast<std::uint8_t>(p[6] & kButtonFire); return true;
}
std::vector<std::uint8_t> encode_welcome_payload(const WelcomeInfo& w) {
  std::vector<std::uint8_t> p; p.reserve(5); p.push_back(w.faction); append_u16(p, w.world_width); append_u16(p, w.world_height); return p;
}
bool decode_welcome_payload(std::span<const std::uint8_t> p, WelcomeInfo& w) {
  if (p.size() != 5) return false;
  w.faction = p[0]; w.world_width = read_u16(p, 1); w.world_height = read_u16(p, 3);
  return (w.faction == 1 || w.faction == 2) && w.world_width > 0 && w.world_height > 0;
}
std::vector<std::uint8_t> encode_snapshot_payload(const WorldSnapshot& s) {
  if (s.players.size() > kMaxPlayers) return {};
  std::vector<std::uint8_t> p; p.reserve(7 + s.players.size() * 14);
  append_u32(p, s.server_tick); append_i16(p, static_cast<std::int16_t>(std::clamp<int>(s.capture_progress, -100, 100)));
  p.push_back(static_cast<std::uint8_t>(s.players.size()));
  for (const auto& q : s.players) {
    append_u32(p, q.id); p.push_back(q.faction); append_i16(p, q.x); append_i16(p, q.y);
    p.push_back(q.health); append_u16(p, q.kills); append_u16(p, q.deaths);
  }
  return p;
}
bool decode_snapshot_payload(std::span<const std::uint8_t> p, WorldSnapshot& s) {
  if (p.size() < 7) return false;
  const auto count = p[6];
  if (count > kMaxPlayers || p.size() != 7 + static_cast<std::size_t>(count) * 14) return false;
  const auto progress = read_i16(p, 4); if (progress < -100 || progress > 100) return false;
  WorldSnapshot candidate; candidate.server_tick = read_u32(p, 0); candidate.capture_progress = progress; candidate.players.reserve(count);
  std::size_t off = 7;
  for (std::size_t i = 0; i < count; ++i) {
    PlayerSnapshot q; q.id = read_u32(p, off); q.faction = p[off + 4]; q.x = read_i16(p, off + 5); q.y = read_i16(p, off + 7);
    q.health = p[off + 9]; q.kills = read_u16(p, off + 10); q.deaths = read_u16(p, off + 12);
    if (q.id == 0 || (q.faction != 1 && q.faction != 2) || q.x < 0 || q.x > kWorldWidth ||
        q.y < 0 || q.y > kWorldHeight || q.health > 100) return false;
    candidate.players.push_back(q); off += 14;
  }
  s = std::move(candidate); return true;
}
} // namespace hng
