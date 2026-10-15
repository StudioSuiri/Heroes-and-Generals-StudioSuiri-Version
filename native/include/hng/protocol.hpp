#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>
namespace hng {
inline constexpr std::uint8_t kProtocolVersion = 1;
inline constexpr std::size_t kPacketHeaderBytes = 16;
inline constexpr std::size_t kMaxDatagramBytes = 1200;
inline constexpr std::size_t kMaxPayloadBytes = kMaxDatagramBytes - kPacketHeaderBytes;
inline constexpr std::size_t kMaxPlayers = 32;
inline constexpr std::uint16_t kWorldWidth = 1600;
inline constexpr std::uint16_t kWorldHeight = 900;
inline constexpr std::uint8_t kButtonFire = 0x01;
enum class PacketType : std::uint8_t { Hello = 1, Welcome = 2, Input = 3, Snapshot = 4, Goodbye = 5 };

// Serial-number arithmetic with wrap-around; distances >= half the domain are old.
inline constexpr bool sequence_is_newer(std::uint32_t incoming, std::uint32_t previous) noexcept {
  const std::uint32_t distance = incoming - previous;
  return distance != 0U && distance < 0x80000000U;
}
struct Packet { PacketType type{}; std::uint32_t sequence{}; std::uint32_t client_id{}; std::vector<std::uint8_t> payload; };
struct ClientInput { std::int8_t move_x{}; std::int8_t move_y{}; std::int16_t aim_x{32767}; std::int16_t aim_y{}; std::uint8_t buttons{}; };
struct WelcomeInfo { std::uint8_t faction{}; std::uint16_t world_width{kWorldWidth}; std::uint16_t world_height{kWorldHeight}; };
struct PlayerSnapshot { std::uint32_t id{}; std::uint8_t faction{}; std::int16_t x{}; std::int16_t y{}; std::uint8_t health{100}; std::uint16_t kills{}; std::uint16_t deaths{}; };
struct WorldSnapshot { std::uint32_t server_tick{}; std::int16_t capture_progress{}; std::vector<PlayerSnapshot> players; };
std::vector<std::uint8_t> encode_packet(PacketType type, std::uint32_t sequence, std::uint32_t client_id, std::span<const std::uint8_t> payload = {});
std::optional<Packet> decode_packet(std::span<const std::uint8_t> bytes);
std::vector<std::uint8_t> encode_input_payload(const ClientInput& input);
bool decode_input_payload(std::span<const std::uint8_t> payload, ClientInput& input);
std::vector<std::uint8_t> encode_welcome_payload(const WelcomeInfo& welcome);
bool decode_welcome_payload(std::span<const std::uint8_t> payload, WelcomeInfo& welcome);
std::vector<std::uint8_t> encode_snapshot_payload(const WorldSnapshot& snapshot);
bool decode_snapshot_payload(std::span<const std::uint8_t> payload, WorldSnapshot& snapshot);
} // namespace hng
