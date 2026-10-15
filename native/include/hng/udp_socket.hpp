#pragma once
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace hng {
struct Endpoint {
  std::uint32_t ipv4_host_order{};
  std::uint16_t port_host_order{};
  friend bool operator==(const Endpoint&, const Endpoint&) = default;
};
struct Datagram {
  Endpoint sender;
  std::vector<std::uint8_t> bytes;
};
class UdpSocket {
public:
  UdpSocket() = default;
  ~UdpSocket();
  UdpSocket(const UdpSocket&) = delete;
  UdpSocket& operator=(const UdpSocket&) = delete;
  bool open_bind(std::string_view ipv4, std::uint16_t port, std::string& error);
  bool send_to(const Endpoint& destination, std::span<const std::uint8_t> bytes);
  std::optional<Datagram> receive();
  void close() noexcept;
  [[nodiscard]] bool is_open() const noexcept;
private:
  static constexpr std::uintptr_t kInvalidSocket = static_cast<std::uintptr_t>(-1);
  std::uintptr_t socket_handle_{kInvalidSocket};
  bool winsock_started_{};
};
std::optional<Endpoint> make_ipv4_endpoint(std::string_view ipv4, std::uint16_t port);
} // namespace hng
