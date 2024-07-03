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

#include <iostream>
#include <string>

#include "libreach/device_id.hpp"
#include "libreach/packet.hpp"
#include "libreach/packet_id.hpp"
#include "libreach/serial_driver.hpp"

/// This example demonstrates how to configure multiple worker threads for processing incoming packets. This
/// configuration can be helpful when processing a large number of packets or when callbacks are computationally
/// expensive.
auto main() -> int
{
  const std::string serial_port = "/dev/ttyUSB0";

  // Set the number of worker threads to 5; this will create 5 worker threads for processing incoming packets.
  const std::size_t n_workers = 5;

  // Set the queue size to 100; this will create a queue with a capacity of 100 packets for storing incoming packets.
  // Modify this value as needed based on the expected number of incoming packets.
  const std::size_t q_size = 100;

  // Create a new serial driver for the Alpha 5
  libreach::SerialDriver driver(serial_port, q_size, n_workers);

  // Create a vector to store the joint positions
  std::vector<float> joint_positions(5, 0.0F);

  // When using multiple worker threads with a non-const callback, it is important to ensure that the shared data is
  // protected
  std::mutex m;

  // Register a callback for the POSITION packet
  driver.register_callback(libreach::PacketId::POSITION, [&m, &joint_positions](const libreach::Packet & packet) {
    const auto position = libreach::deserialize<float>(packet);

    const std::lock_guard<std::mutex> lock(m);
    joint_positions[static_cast<std::size_t>(packet.device_id()) - 1] = position;
  });

  // Request position data at 100 Hz for each joint
  for (std::size_t i = 1; i <= 5; ++i) {
    driver.request_at_rate(libreach::PacketId::POSITION, static_cast<std::uint8_t>(i), std::chrono::milliseconds(10));
  }

  // Print out the joint positions
  while (true) {
    for (std::size_t i = 0; i < joint_positions.size(); ++i) {
      std::cout << "Joint " << i + 1 << " position: " << joint_positions[i] << "\n";
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}
