// Copyright (c) 2024 Evan Palmer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), limited
// exclusively to use with products produced by Reach Robotics Pty Ltd, subject to
// the following conditions:
//
// The Software may only be used in conjunction with products manufactured or
// developed by Reach Robotics Pty Ltd.
//
// Redistributions or use of the Software in any other context, including but
// not limited to, integration, combination, or use with other products or
// software, are strictly prohibited without prior written authorization from Reach
// Robotics Pty Ltd.
//
// All copies of the Software, in whole or in part, must retain this notice and
// the above copyright notice.
//
// THIS SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT. IN NO EVENT SHALL REACH ROBOTICS
// PTY LTD BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM, OUT OF, OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "libreach/client.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include "libreach/packet_id.hpp"

namespace libreach::protocol
{

Client::Client(std::function<void(const std::vector<Packet> &)> && callback, std::chrono::seconds session_timeout)
: packet_callback_(std::forward<std::function<void(const std::vector<Packet> &)>>(callback))
{
  running_.store(true);

  set_last_heartbeat(std::chrono::steady_clock::now());
  heartbeat_monitor_thread_ = std::thread([this, session_timeout] {
    while (running_.load()) {
      check_heartbeat(session_timeout);
      std::this_thread::sleep_for(session_timeout / 2);
    }
  });
}

auto Client::start_polling_connection(std::uint16_t max_bytes_to_read) -> void
{
  polling_thread_ = std::thread(&Client::poll_connection, this, max_bytes_to_read);

  disable_heartbeat();
  enable_heartbeat(1);  // Set the heartbeat rate to its minimum value (1 Hz)
}

auto Client::shutdown_client() -> void
{
  running_.store(false);

  if (polling_thread_.joinable()) {
    polling_thread_.join();
  }

  if (heartbeat_monitor_thread_.joinable()) {
    heartbeat_monitor_thread_.join();
  }
}

auto Client::connected() const -> bool
{
  const std::lock_guard<std::mutex> lock(connection_state_lock_);
  return running_.load() && connection_state_ == ConnectionState::CONNECTED;
}

auto Client::send_packet(const Packet & packet) const -> void
{
  if (write_to_connection(encode_packet(packet)) < 0) {
    throw std::runtime_error("Failed to send packet; the connection was likely lost.");
  }
}

auto Client::enable_heartbeat(unsigned int frequency) const -> void
{
  // Request the model number as the heartbeat because there isn't an official heartbeat message
  send_packet(Packet(PacketId::HEARTBEAT_SET, 0xFF, {static_cast<std::uint8_t>(PacketId::MODEL_NUMBER)}));
  set_heartbeat_rate(frequency);
}

auto Client::disable_heartbeat() const -> void { set_heartbeat_rate(0); }

auto Client::set_heartbeat_rate(unsigned int frequency) const -> void
{
  send_packet(Packet(PacketId::HEARTBEAT_FREQUENCY, 0xFF, {static_cast<std::uint8_t>(frequency)}));
}

auto Client::set_last_heartbeat(std::chrono::time_point<std::chrono::steady_clock> t) -> void
{
  const std::lock_guard<std::mutex> lock(last_heartbeat_lock_);
  last_heartbeat_ = t;
}

auto Client::check_heartbeat(std::chrono::seconds timeout) -> void
{
  const std::scoped_lock lock(last_heartbeat_lock_, connection_state_lock_);

  const bool timeout_occurred = std::chrono::steady_clock::now() - last_heartbeat_ > timeout;

  if (timeout_occurred && connection_state_ == ConnectionState::CONNECTED) {
    std::stringstream ss;
    ss << "Client timeout occurred; heartbeat has not been received in the last " << timeout.count() << " seconds\n";
    std::cout << ss.str();
  } else if (!timeout_occurred && connection_state_ == ConnectionState::DISCONNECTED) {
    std::cout << "Client connection established\n";
  }

  connection_state_ = timeout_occurred ? ConnectionState::DISCONNECTED : ConnectionState::CONNECTED;
}

auto Client::poll_connection(std::uint16_t max_bytes_to_read) -> void
{
  std::deque<std::uint8_t> buffer(max_bytes_to_read);
  std::size_t n_bytes_to_read = max_bytes_to_read;

  while (running_.load()) {
    if (read_bytes(buffer, n_bytes_to_read) < 0) {
      std::cout << "Failed to read from the robot; the connection was likely lost.\n";
    }

    auto last_delim = std::find(buffer.rbegin(), buffer.rend(), PACKET_DELIMITER);

    if (last_delim != buffer.rend()) {
      try {
        const std::vector<Packet> packets = decode_packets({buffer.begin(), last_delim.base()});

        if (!packets.empty()) {
          auto it = std::find_if(packets.begin(), packets.end(), [](const Packet & packet) {
            return packet.packet_id() == PacketId::MODEL_NUMBER;
          });

          if (it != packets.end()) {
            set_last_heartbeat(std::chrono::steady_clock::now());
          }

          packet_callback_(packets);
        }

        buffer.erase(buffer.begin(), last_delim.base());
      }
      catch (const std::exception & e) {
        std::cout << "An error occurred while attempting to decode a packet: " << e.what() << "\n";
        buffer.clear();
      }
    }

    n_bytes_to_read = max_bytes_to_read - buffer.size();
  }
}

}  // namespace libreach::protocol