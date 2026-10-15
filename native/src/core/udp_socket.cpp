#include "hng/udp_socket.hpp"
#include "hng/protocol.hpp"
#include <array>
#include <string>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace hng {
namespace {
constexpr std::uintptr_t kInvalidSocket = static_cast<std::uintptr_t>(-1);
#ifdef _WIN32
SOCKET native_socket(std::uintptr_t v) { return static_cast<SOCKET>(v); }
#else
int native_socket(std::uintptr_t v) { return static_cast<int>(v); }
#endif
bool parse_address(std::string_view s, std::uint32_t& host_order) {
  const std::string terminated(s); in_addr address{};
#ifdef _WIN32
  if (InetPtonA(AF_INET, terminated.c_str(), &address) != 1) return false;
#else
  if (inet_pton(AF_INET, terminated.c_str(), &address) != 1) return false;
#endif
  host_order = ntohl(address.s_addr); return true;
}
}

UdpSocket::~UdpSocket() { close(); }
bool UdpSocket::open_bind(std::string_view ipv4, std::uint16_t port, std::string& error) {
  close();
#ifdef _WIN32
  WSADATA data{};
  const int startup = WSAStartup(MAKEWORD(2, 2), &data);
  if (startup != 0) { error = "WSAStartup failed: " + std::to_string(startup); return false; }
  winsock_started_ = true;
  const SOCKET handle = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (handle == INVALID_SOCKET) { error = "socket failed: " + std::to_string(WSAGetLastError()); close(); return false; }
  socket_handle_ = static_cast<std::uintptr_t>(handle);
  u_long nonblocking = 1;
  if (ioctlsocket(handle, FIONBIO, &nonblocking) != 0) { error = "ioctlsocket failed: " + std::to_string(WSAGetLastError()); close(); return false; }
#else
  const int handle = ::socket(AF_INET, SOCK_DGRAM, 0);
  if (handle < 0) { error = "socket() failed"; return false; }
  socket_handle_ = static_cast<std::uintptr_t>(handle);
  const int flags = fcntl(handle, F_GETFL, 0);
  if (flags < 0 || fcntl(handle, F_SETFL, flags | O_NONBLOCK) < 0) { error = "fcntl O_NONBLOCK failed"; close(); return false; }
#endif
  std::uint32_t address_host{};
  if (!parse_address(ipv4, address_host)) { error = "Bind address must be an IPv4 literal"; close(); return false; }
  sockaddr_in address{}; address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(address_host); address.sin_port = htons(port);
#ifdef _WIN32
  if (::bind(native_socket(socket_handle_), reinterpret_cast<const sockaddr*>(&address), static_cast<int>(sizeof(address))) == SOCKET_ERROR) {
    error = "bind() failed: " + std::to_string(WSAGetLastError()); close(); return false;
  }
#else
  if (::bind(native_socket(socket_handle_), reinterpret_cast<const sockaddr*>(&address), static_cast<socklen_t>(sizeof(address))) < 0) {
    error = "bind() failed; check address, port and permissions"; close(); return false;
  }
#endif
  return true;
}
bool UdpSocket::send_to(const Endpoint& dst, std::span<const std::uint8_t> bytes) {
  if (!is_open() || bytes.empty() || bytes.size() > kMaxDatagramBytes) return false;
  sockaddr_in address{}; address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(dst.ipv4_host_order); address.sin_port = htons(dst.port_host_order);
#ifdef _WIN32
  const int sent = ::sendto(native_socket(socket_handle_), reinterpret_cast<const char*>(bytes.data()),
    static_cast<int>(bytes.size()), 0, reinterpret_cast<const sockaddr*>(&address), static_cast<int>(sizeof(address)));
  return sent == static_cast<int>(bytes.size());
#else
  const auto sent = ::sendto(native_socket(socket_handle_), bytes.data(), bytes.size(), 0,
    reinterpret_cast<const sockaddr*>(&address), static_cast<socklen_t>(sizeof(address)));
  return sent == static_cast<ssize_t>(bytes.size());
#endif
}
std::optional<Datagram> UdpSocket::receive() {
  if (!is_open()) return std::nullopt;
  std::array<std::uint8_t, kMaxDatagramBytes + 1> buf{};
  sockaddr_in sender{};
#ifdef _WIN32
  int len = static_cast<int>(sizeof(sender));
  const int got = ::recvfrom(native_socket(socket_handle_), reinterpret_cast<char*>(buf.data()),
    static_cast<int>(buf.size()), 0, reinterpret_cast<sockaddr*>(&sender), &len);
  if (got <= 0) return std::nullopt;
#else
  socklen_t len = static_cast<socklen_t>(sizeof(sender));
  const auto got = ::recvfrom(native_socket(socket_handle_), buf.data(), buf.size(), 0,
    reinterpret_cast<sockaddr*>(&sender), &len);
  if (got <= 0) return std::nullopt;
#endif
  Datagram out; out.sender.ipv4_host_order = ntohl(sender.sin_addr.s_addr);
  out.sender.port_host_order = ntohs(sender.sin_port);
  out.bytes.assign(buf.begin(), buf.begin() + got);
  return out;
}
void UdpSocket::close() noexcept {
  if (socket_handle_ != kInvalidSocket) {
#ifdef _WIN32
    closesocket(native_socket(socket_handle_));
#else
    ::close(native_socket(socket_handle_));
#endif
    socket_handle_ = kInvalidSocket;
  }
#ifdef _WIN32
  if (winsock_started_) { WSACleanup(); winsock_started_ = false; }
#endif
}
bool UdpSocket::is_open() const noexcept { return socket_handle_ != kInvalidSocket; }
std::optional<Endpoint> make_ipv4_endpoint(std::string_view ip, std::uint16_t port) {
  std::uint32_t host{}; if (!parse_address(ip, host)) return std::nullopt;
  return Endpoint{host, port};
}
} // namespace hng
