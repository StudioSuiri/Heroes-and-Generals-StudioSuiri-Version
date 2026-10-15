#include "hng/protocol.hpp"
#include "hng/simulation.hpp"
#include "hng/udp_socket.hpp"

#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace {
using Clock = std::chrono::steady_clock;
volatile std::sig_atomic_t g_running = 1;
void stop_server(int) { g_running = 0; }

bool parse_unsigned(std::string_view text, unsigned long& out) {
  try {
    std::size_t consumed = 0;
    out = std::stoul(std::string(text), &consumed, 10);
    return consumed == text.size();
  } catch (...) { return false; }
}
struct Peer {
  hng::Endpoint endpoint;
  std::uint32_t player_id{};
  std::uint32_t last_input_sequence{};
  bool has_input_sequence{};
  Clock::time_point last_seen{Clock::now()};
};
bool sequence_is_newer(std::uint32_t a, std::uint32_t b) {
  return a != b && static_cast<std::int32_t>(a - b) > 0;
}
void send_welcome(hng::UdpSocket& socket, const Peer& peer) {
  const hng::WelcomeInfo info{
    static_cast<std::uint8_t>((peer.player_id % 2U == 1U) ? 1U : 2U),
    hng::kWorldWidth, hng::kWorldHeight
  };
  const auto payload = hng::encode_welcome_payload(info);
  const auto packet = hng::encode_packet(hng::PacketType::Welcome, 0, peer.player_id, payload);
  socket.send_to(peer.endpoint, packet);
}
}

int main(int argc, char** argv) {
  std::string bind_address = "0.0.0.0";
  unsigned long port_value = 26000;
  unsigned long tick_rate = 30;
  for (int i = 1; i < argc; ++i) {
    const std::string_view arg(argv[i]);
    if ((arg == "--bind" || arg == "--port" || arg == "--tick-rate") && i + 1 >= argc) {
      std::cerr << "Missing value after " << arg << "\n"; return 2;
    }
    if (arg == "--bind") bind_address = argv[++i];
    else if (arg == "--port") {
      if (!parse_unsigned(argv[++i], port_value) || port_value == 0 || port_value > 65535) {
        std::cerr << "Invalid port; expected 1..65535.\n"; return 2;
      }
    } else if (arg == "--tick-rate") {
      if (!parse_unsigned(argv[++i], tick_rate) || tick_rate < 10 || tick_rate > 60) {
        std::cerr << "Invalid tick rate; expected 10..60.\n"; return 2;
      }
    } else if (arg == "--help") {
      std::cout << "hng-server [--bind 0.0.0.0] [--port 26000] [--tick-rate 30]\n"; return 0;
    } else { std::cerr << "Unknown argument: " << arg << "\n"; return 2; }
  }

  std::signal(SIGINT, stop_server);
#ifdef SIGTERM
  std::signal(SIGTERM, stop_server);
#endif
  hng::UdpSocket socket;
  std::string error;
  if (!socket.open_bind(bind_address, static_cast<std::uint16_t>(port_value), error)) {
    std::cerr << "Could not open UDP socket: " << error << "\n"; return 1;
  }

  hng::Simulation simulation;
  std::vector<Peer> peers;
  const auto step = std::chrono::duration_cast<Clock::duration>(
    std::chrono::duration<double>(1.0 / static_cast<double>(tick_rate)));
  auto next_tick = Clock::now();

  std::cout << "HnG Native headless server 0.1.0\n"
    << "Bind: " << bind_address << ":" << port_value << " | tick rate: " << tick_rate
    << " | max players: " << hng::kMaxPlayers << "\n"
    << "LAN prototype only. No authentication, encryption, anti-cheat or internet hardening.\n";

  while (g_running != 0) {
    while (auto datagram = socket.receive()) {
      const auto packet = hng::decode_packet(datagram->bytes);
      if (!packet) continue;
      const auto now = Clock::now();
      auto it = std::find_if(peers.begin(), peers.end(), [&](const Peer& peer) {
        return peer.endpoint == datagram->sender;
      });

      if (packet->type == hng::PacketType::Hello && packet->client_id == 0) {
        if (it == peers.end()) {
          if (peers.size() >= hng::kMaxPlayers) continue;
          const auto id = simulation.add_player();
          if (id == 0) continue;
          peers.push_back(Peer{datagram->sender, id, 0, false, now});
          it = std::prev(peers.end());
          std::cout << "Player " << id << " joined (" << peers.size() << "/" << hng::kMaxPlayers << ")\n";
        }
        it->last_seen = now;
        send_welcome(socket, *it);
        continue;
      }

      if (it == peers.end() || packet->client_id != it->player_id) continue;
      it->last_seen = now;
      if (packet->type == hng::PacketType::Goodbye) {
        std::cout << "Player " << it->player_id << " left\n";
        simulation.remove_player(it->player_id);
        peers.erase(it);
        continue;
      }
      if (packet->type == hng::PacketType::Input &&
          (!it->has_input_sequence || sequence_is_newer(packet->sequence, it->last_input_sequence))) {
        hng::ClientInput input;
        if (hng::decode_input_payload(packet->payload, input)) {
          simulation.set_input(it->player_id, input);
          it->last_input_sequence = packet->sequence;
          it->has_input_sequence = true;
        }
      }
    }

    const auto now = Clock::now();
    std::erase_if(peers, [&](const Peer& peer) {
      if (now - peer.last_seen <= std::chrono::seconds(10)) return false;
      simulation.remove_player(peer.player_id);
      std::cout << "Player " << peer.player_id << " timed out\n";
      return true;
    });

    simulation.tick(1.0F / static_cast<float>(tick_rate));
    const auto snapshot = simulation.snapshot();
    const auto payload = hng::encode_snapshot_payload(snapshot);
    for (const auto& peer : peers) {
      const auto bytes = hng::encode_packet(hng::PacketType::Snapshot,
        snapshot.server_tick, peer.player_id, payload);
      socket.send_to(peer.endpoint, bytes);
    }

    next_tick += step;
    const auto after = Clock::now();
    if (next_tick < after) next_tick = after;
    else std::this_thread::sleep_until(next_tick);
  }

  socket.close();
  std::cout << "Server stopped.\n";
  return 0;
}
