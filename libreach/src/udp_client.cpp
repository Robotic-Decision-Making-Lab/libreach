// Copyright 2024, Evan Palmer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "libreach/udp_client.hpp"

#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>

namespace libreach::protocol
{

UdpClient::UdpClient(
  const std::string & addr,
  std::uint16_t port,
  std::function<void(const std::vector<Packet> &)> && callback,
  std::chrono::seconds session_timeout)
: Client(std::forward<std::function<void(const std::vector<Packet> &)>>(callback), session_timeout)
{
  socket_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_ < 0) {
    throw std::runtime_error("Failed to open UDP socket");
  }

  struct sockaddr_in sockaddr;

  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(port);
  sockaddr.sin_addr.s_addr = inet_addr(addr.c_str());

  if (sockaddr.sin_addr.s_addr == INADDR_NONE) {
    throw std::runtime_error("Invalid socket address " + addr);
  }

  if (connect(socket_, reinterpret_cast<struct sockaddr *>(&sockaddr), sizeof(sockaddr)) < 0) {
    throw std::runtime_error("Failed to connect to UDP socket");
  }

  start_polling_connection();
}

UdpClient::~UdpClient()
{
  shutdown_client();
  close(socket_);
}

auto UdpClient::write_to_connection(const std::vector<std::uint8_t> & data) const -> ssize_t
{
  return send(socket_, data.data(), data.size(), 0);
}

auto UdpClient::read_bytes(std::deque<std::uint8_t> & buffer, std::size_t n_bytes) const -> ssize_t
{
  std::vector<std::uint8_t> data(n_bytes);
  const ssize_t n_read = recv(socket_, data.data(), data.size(), 0);
  buffer.insert(buffer.end(), data.begin(), data.begin() + n_read);
  return n_read;
}

}  // namespace libreach::protocol
