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

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

#include "libreach/packet_id.hpp"

namespace libreach
{

class Packet
{
public:
  /// Create a new packet given the packet ID, device ID, and (unencoded) data.
  Packet(PacketId packet_id, std::uint8_t device_id, std::vector<std::uint8_t> data);

  [[nodiscard]] auto packet_id() const -> PacketId;

  [[nodiscard]] auto device_id() const -> std::uint8_t;

  [[nodiscard]] auto data() const -> std::vector<std::uint8_t>;

  [[nodiscard]] auto data_size() const -> std::size_t;

private:
  PacketId packet_id_;
  std::uint8_t device_id_;
  std::vector<std::uint8_t> data_;
};

/// Deserialize a packet's data into a given type.
template <typename T>
[[nodiscard]] inline auto deserialize(const Packet & packet) -> T
{
  if (packet.data_size() != sizeof(T)) {
    throw std::invalid_argument("Cannot deserialize packet data into the requested type due to mismatched sizes.");
  }
  return *reinterpret_cast<const T *>(packet.data().data());
}

namespace protocol
{

/// The delimiter used to separate packets in a byte stream
const std::uint8_t PACKET_DELIMITER = 0x00;

/// Encode a packet into a byte stream
auto encode_packet(const Packet & packet) -> std::vector<std::uint8_t>;

/// Decode a packet from a byte stream
auto decode_packet(const std::vector<std::uint8_t> & data) -> Packet;

/// Decode multiple packets from a byte stream
auto decode_packets(const std::vector<std::uint8_t> & data) -> std::vector<Packet>;

}  // namespace protocol

}  // namespace libreach
