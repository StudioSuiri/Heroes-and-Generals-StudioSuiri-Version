#include "hng/protocol.hpp"
#include "hng/simulation.hpp"
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
void require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}
const hng::PlayerSnapshot* by_id(const hng::WorldSnapshot& state, std::uint32_t id) {
  for (const auto& p : state.players) if (p.id == id) return &p;
  return nullptr;
}
void packet_roundtrip_and_validation() {
  require(hng::sequence_is_newer(1U, 0U), "next sequence should be newer");
  require(!hng::sequence_is_newer(0U, 0U), "equal sequence should not be newer");
  require(hng::sequence_is_newer(0U, 0xffffffffU), "sequence comparison should handle wrap-around");
  require(!hng::sequence_is_newer(0xffffffffU, 0U), "pre-wrap old sequence should be rejected");
  require(!hng::sequence_is_newer(0x80000000U, 0U), "half-range ambiguity should not be treated as newer");
  hng::ClientInput input; input.move_x = -1; input.move_y = 1;
  input.aim_x = -12345; input.aim_y = 23456; input.buttons = hng::kButtonFire;
  const auto payload = hng::encode_input_payload(input);
  const auto bytes = hng::encode_packet(hng::PacketType::Input, 42, 7, payload);
  const auto packet = hng::decode_packet(bytes);
  require(packet.has_value(), "valid packet should decode");
  require(packet->sequence == 42 && packet->client_id == 7, "header fields must roundtrip");
  hng::ClientInput restored;
  require(hng::decode_input_payload(packet->payload, restored), "input payload should decode");
  require(restored.move_x == -1 && restored.move_y == 1, "axes must roundtrip");
  require(restored.aim_x == input.aim_x && restored.aim_y == input.aim_y, "aim must roundtrip");
  require(restored.buttons == hng::kButtonFire, "button mask must roundtrip");
  auto bad_magic = bytes; bad_magic[0] = 'X';
  require(!hng::decode_packet(bad_magic), "bad magic must reject");
  auto bad_version = bytes; bad_version[4] = 99;
  require(!hng::decode_packet(bad_version), "unknown version must reject");
  auto bad_type = bytes; bad_type[5] = 99;
  require(!hng::decode_packet(bad_type), "unknown message type must reject");
  auto bad_length = bytes; bad_length[6] = 255;
  require(!hng::decode_packet(bad_length), "mismatched payload length must reject");
  const std::vector<std::uint8_t> short_packet{1, 2, 3};
  require(!hng::decode_packet(short_packet), "short packet must reject");
  const std::vector<std::uint8_t> large(hng::kMaxDatagramBytes + 1, 0);
  require(!hng::decode_packet(large), "oversized datagram must reject");
  const std::vector<std::uint8_t> payload_too_large(hng::kMaxPayloadBytes + 1, 0);
  require(hng::encode_packet(hng::PacketType::Input, 1, 1, payload_too_large).empty(), "oversized payload must not encode");
}
void snapshot_roundtrip() {
  hng::WorldSnapshot original{1337, -63, {{1, 1, 120, 200, 100, 3, 1}, {2, 2, 1480, 600, 66, 0, 4}}};
  const auto payload = hng::encode_snapshot_payload(original);
  hng::WorldSnapshot restored;
  require(hng::decode_snapshot_payload(payload, restored), "snapshot must decode");
  require(restored.server_tick == original.server_tick, "tick roundtrips");
  require(restored.capture_progress == original.capture_progress, "capture progress roundtrips");
  require(restored.players.size() == 2 && restored.players[1].health == 66 && restored.players[1].deaths == 4, "player state roundtrips");
  auto corrupt = payload; corrupt[6] = 255;
  require(!hng::decode_snapshot_payload(corrupt, restored), "invalid count rejects");
}
void movement_and_capture() {
  hng::Simulation sim;
  const auto id = sim.add_player();
  require(id != 0, "first player created");
  hng::ClientInput input; input.move_x = 1;
  sim.set_input(id, input);
  for (int i = 0; i < 95; ++i) sim.tick(1.0F / 30.0F);
  input.move_x = 0; input.move_y = 1; sim.set_input(id, input);
  for (int i = 0; i < 25; ++i) sim.tick(1.0F / 30.0F);
  input.move_y = 0; input.move_x = 1; sim.set_input(id, input);
  for (int i = 0; i < 18; ++i) sim.tick(1.0F / 30.0F);
  sim.set_input(id, hng::ClientInput{});
  for (int i = 0; i < 30; ++i) sim.tick(1.0F / 30.0F);
  const auto state = sim.snapshot();
  const auto* p = by_id(state, id);
  require(p != nullptr, "player appears in snapshot");
  require(p->x > 0 && p->x < hng::kWorldWidth && p->y > 0 && p->y < hng::kWorldHeight, "player stays in bounds");
  require(state.capture_progress > 0, "uncontested faction one should capture");
}
void combat_hit_and_capacity() {
  hng::Simulation sim;
  const auto attacker = sim.add_player(), target = sim.add_player();
  require(attacker && target, "two players join");

  // Deliberately put the two factions within the prototype rifle's test range.
  // The previous scenario stopped the target hundreds of units beyond that range,
  // so its expected damage assertion could never be satisfied.
  hng::ClientInput attacker_move;
  attacker_move.move_x = 1;
  sim.set_input(attacker, attacker_move);
  for (int i = 0; i < 118; ++i) sim.tick(1.0F / 30.0F);
  sim.set_input(attacker, hng::ClientInput{});

  hng::ClientInput target_move;
  target_move.move_y = -1;
  sim.set_input(target, target_move);
  for (int i = 0; i < 20; ++i) sim.tick(1.0F / 30.0F);
  target_move.move_y = 0;
  target_move.move_x = -1;
  sim.set_input(target, target_move);
  for (int i = 0; i < 80; ++i) sim.tick(1.0F / 30.0F);
  sim.set_input(target, hng::ClientInput{});

  hng::ClientInput fire;
  fire.aim_x = 32767;
  fire.aim_y = 0;
  fire.buttons = hng::kButtonFire;
  sim.set_input(attacker, fire);
  sim.tick(1.0F / 30.0F);

  auto state = sim.snapshot();
  const auto* target_state = by_id(state, target);
  require(target_state != nullptr, "target remains in snapshot");
  require(target_state->health == 66, "hitscan applies server-side damage");
  require(by_id(state, attacker)->kills == 0, "nonlethal hit grants no kill");
  sim.set_input(attacker, hng::ClientInput{});
  for (std::size_t i = 2; i < hng::kMaxPlayers; ++i) require(sim.add_player() != 0, "players below capacity admitted");
  require(sim.add_player() == 0, "capacity is enforced");
}
}
int main() {
  try {
    packet_roundtrip_and_validation();
    snapshot_roundtrip();
    movement_and_capture();
    combat_hit_and_capacity();
    std::cout << "Native core tests passed.\n"; return 0;
  } catch (const std::exception& e) {
    std::cerr << "Native core test failure: " << e.what() << "\n"; return 1;
  }
}
