#include "hng/protocol.hpp"
#include "hng/udp_socket.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>

namespace {
using Clock = std::chrono::steady_clock;
bool parse_port(std::string_view text, std::uint16_t& port) {
  try {
    std::size_t consumed = 0; const auto value = std::stoul(std::string(text), &consumed, 10);
    if (consumed != text.size() || value == 0 || value > 65535) return false;
    port = static_cast<std::uint16_t>(value); return true;
  } catch (...) { return false; }
}
const hng::PlayerSnapshot* find_player(const hng::WorldSnapshot& state, std::uint32_t id) {
  const auto it = std::find_if(state.players.begin(), state.players.end(), [id](const auto& p) { return p.id == id; });
  return it == state.players.end() ? nullptr : &*it;
}
void fill_rect(SDL_Renderer* renderer, float x, float y, float w, float h,
               Uint8 r, Uint8 g, Uint8 b) {
  SDL_SetRenderDrawColor(renderer, r, g, b, 255);
  const SDL_FRect rect{x, y, w, h};
  SDL_RenderFillRect(renderer, &rect);
}
}

int main(int argc, char** argv) {
  std::string server_ip = "127.0.0.1";
  std::uint16_t server_port = 26000;
  for (int i = 1; i < argc; ++i) {
    const std::string_view arg(argv[i]);
    if ((arg == "--connect" || arg == "--port") && i + 1 >= argc) {
      std::cerr << "Missing value after " << arg << "\n"; return 2;
    }
    if (arg == "--connect") server_ip = argv[++i];
    else if (arg == "--port") {
      if (!parse_port(argv[++i], server_port)) { std::cerr << "Invalid port; expected 1..65535.\n"; return 2; }
    } else if (arg == "--help") {
      std::cout << "hng-client [--connect 127.0.0.1] [--port 26000]\n"; return 0;
    } else { std::cerr << "Unknown argument: " << arg << "\n"; return 2; }
  }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n"; return 1;
  }
  SDL_Window* window = nullptr; SDL_Renderer* renderer = nullptr;
  if (!SDL_CreateWindowAndRenderer("HnG Native — connecting", 1280, 720,
      SDL_WINDOW_RESIZABLE, &window, &renderer)) {
    std::cerr << "Window creation failed: " << SDL_GetError() << "\n"; SDL_Quit(); return 1;
  }

  hng::UdpSocket socket;
  std::string socket_error;
  if (!socket.open_bind("0.0.0.0", 0, socket_error)) {
    std::cerr << "Could not open client socket: " << socket_error << "\n";
    SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit(); return 1;
  }
  const auto server = hng::make_ipv4_endpoint(server_ip, server_port);
  if (!server) {
    std::cerr << "--connect currently accepts an IPv4 literal, e.g. 192.168.1.20\n";
    socket.close(); SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit(); return 2;
  }

  std::cout << "HnG Native standalone client. Server " << server_ip << ":" << server_port << "\n"
    << "WASD/arrows move | mouse aims | left mouse fires | Esc quits\n";
  bool running = true, connected = false;
  std::uint32_t player_id = 0, input_sequence = 0;
  std::uint8_t faction = 0;
  hng::WorldSnapshot world;
  auto next_hello = Clock::now();
  auto next_input = Clock::now();
  auto last_title_update = Clock::now();

  while (running) {
    SDL_Event event{};
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT ||
          (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE && !event.key.repeat)) {
        running = false;
      }
    }

    while (auto datagram = socket.receive()) {
      const auto packet = hng::decode_packet(datagram->bytes);
      if (!packet) continue;
      if (packet->type == hng::PacketType::Welcome && packet->client_id != 0) {
        hng::WelcomeInfo welcome;
        if (hng::decode_welcome_payload(packet->payload, welcome)) {
          player_id = packet->client_id; faction = welcome.faction; connected = true;
        }
      } else if (packet->type == hng::PacketType::Snapshot &&
                 (packet->client_id == player_id || player_id == 0)) {
        hng::WorldSnapshot received;
        if (hng::decode_snapshot_payload(packet->payload, received)) world = std::move(received);
      }
    }

    const auto now = Clock::now();
    if (now >= next_hello) {
      const auto hello = hng::encode_packet(hng::PacketType::Hello, 0, 0);
      socket.send_to(*server, hello);
      next_hello = now + std::chrono::seconds(2);
    }

    hng::ClientInput input;
    const bool* keys = SDL_GetKeyboardState(nullptr);
    input.move_x = static_cast<std::int8_t>(
      (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT] ? 1 : 0) -
      (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT] ? 1 : 0));
    input.move_y = static_cast<std::int8_t>(
      (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN] ? 1 : 0) -
      (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP] ? 1 : 0));

    float mx = 0.0F, my = 0.0F;
    const auto mouse = SDL_GetMouseState(&mx, &my);
    if ((mouse & SDL_BUTTON_LMASK) != 0) input.buttons |= hng::kButtonFire;
    int window_w = 1280, window_h = 720;
    SDL_GetWindowSize(window, &window_w, &window_h);
    const float sx = static_cast<float>(window_w) / hng::kWorldWidth;
    const float sy = static_cast<float>(window_h) / hng::kWorldHeight;
    const auto* self = connected ? find_player(world, player_id) : nullptr;
    const float player_x = self ? static_cast<float>(self->x) : static_cast<float>(hng::kWorldWidth) / 2.0F;
    const float player_y = self ? static_cast<float>(self->y) : static_cast<float>(hng::kWorldHeight) / 2.0F;
    float ax = mx / std::max(sx, 0.001F) - player_x;
    float ay = my / std::max(sy, 0.001F) - player_y;
    const float aim_len = std::sqrt(ax * ax + ay * ay);
    if (aim_len > 0.01F) {
      ax /= aim_len; ay /= aim_len;
      input.aim_x = static_cast<std::int16_t>(std::lround(std::clamp(ax, -1.0F, 1.0F) * 32767.0F));
      input.aim_y = static_cast<std::int16_t>(std::lround(std::clamp(ay, -1.0F, 1.0F) * 32767.0F));
    }
    if (connected && player_id != 0 && now >= next_input) {
      const auto payload = hng::encode_input_payload(input);
      const auto wire = hng::encode_packet(hng::PacketType::Input, ++input_sequence, player_id, payload);
      socket.send_to(*server, wire);
      next_input = now + std::chrono::milliseconds(33);
    }

    SDL_SetRenderDrawColor(renderer, 28, 37, 32, 255);
    SDL_RenderClear(renderer);
    for (int x = 0; x <= hng::kWorldWidth; x += 100) {
      SDL_SetRenderDrawColor(renderer, 43, 54, 46, 255);
      SDL_RenderLine(renderer, static_cast<float>(x) * sx, 0.0F,
        static_cast<float>(x) * sx, static_cast<float>(window_h));
    }
    for (int y = 0; y <= hng::kWorldHeight; y += 100) {
      SDL_SetRenderDrawColor(renderer, 43, 54, 46, 255);
      SDL_RenderLine(renderer, 0.0F, static_cast<float>(y) * sy,
        static_cast<float>(window_w), static_cast<float>(y) * sy);
    }

    const float cx = static_cast<float>(hng::kWorldWidth) * sx / 2.0F;
    const float cy = static_cast<float>(hng::kWorldHeight) * sy / 2.0F;
    const SDL_FRect zone{cx - 120.0F * sx, cy - 120.0F * sy, 240.0F * sx, 240.0F * sy};
    SDL_SetRenderDrawColor(renderer, 215, 205, 165, 255);
    SDL_RenderRect(renderer, &zone);
    fill_rect(renderer, cx - 14.0F * sx, cy - 14.0F * sy, 28.0F * sx, 28.0F * sy, 170, 158, 120);

    const SDL_FRect cap_bar{static_cast<float>(window_w) / 2.0F - 100.0F, 22.0F, 200.0F, 12.0F};
    fill_rect(renderer, cap_bar.x, cap_bar.y, cap_bar.w, cap_bar.h, 55, 58, 56);
    const float cap = std::abs(static_cast<float>(world.capture_progress)) / 100.0F;
    if (world.capture_progress > 0) fill_rect(renderer, cap_bar.x + 100.0F, cap_bar.y, 100.0F * cap, 12.0F, 65, 155, 220);
    else if (world.capture_progress < 0) fill_rect(renderer, cap_bar.x + 100.0F - 100.0F * cap, cap_bar.y, 100.0F * cap, 12.0F, 220, 82, 70);

    for (const auto& p : world.players) {
      const float x = static_cast<float>(p.x) * sx, y = static_cast<float>(p.y) * sy;
      Uint8 r = p.faction == 1 ? 65 : 220, g = p.faction == 1 ? 155 : 82, b = p.faction == 1 ? 220 : 70;
      if (p.id == player_id) { r = 246; g = 222; b = 104; }
      if (p.health == 0) { r = 100; g = 100; b = 100; }
      fill_rect(renderer, x - 9.0F, y - 9.0F, 18.0F, 18.0F, r, g, b);
      fill_rect(renderer, x - 10.0F, y - 15.0F, 20.0F, 3.0F, 45, 45, 45);
      fill_rect(renderer, x - 10.0F, y - 15.0F, 20.0F * static_cast<float>(p.health) / 100.0F, 3.0F, 90, 200, 95);
    }
    SDL_RenderPresent(renderer);

    if (now - last_title_update > std::chrono::seconds(1)) {
      const std::string title = connected
        ? "HnG Native | player " + std::to_string(player_id) + " | faction " + std::to_string(faction) +
          " | players " + std::to_string(world.players.size())
        : "HnG Native | connecting to " + server_ip + ":" + std::to_string(server_port);
      SDL_SetWindowTitle(window, title.c_str());
      last_title_update = now;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  if (player_id != 0) {
    const auto goodbye = hng::encode_packet(hng::PacketType::Goodbye, ++input_sequence, player_id);
    socket.send_to(*server, goodbye);
  }
  socket.close(); SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit();
  return 0;
}
