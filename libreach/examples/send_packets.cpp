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

// This example demonstrates the different ways to send packets to a robot using libreach.
auto main() -> int
{
  const std::string serial_port = "/dev/ttyUSB0";

  // Create a new serial driver for the Alpha 5
  const libreach::SerialDriver driver(serial_port);

  // Set the desired position for joint A (rad)
  const float desired_position = 0.0;

  const auto joint_a = static_cast<std::uint8_t>(libreach::Alpha5DeviceId::JOINT_A);

  // 1. Helper functions have been provided to execute a variety of command commands such as setting the position
  driver.set_position(joint_a, desired_position);

  // 2. If a helper function does not exist for a specific command, packets can be sent directly to the robot
  std::vector<std::uint8_t> bytes(sizeof(desired_position));
  std::memcpy(bytes.data(), &desired_position, sizeof(desired_position));

  driver.send_packet(libreach::PacketId::POSITION, joint_a, bytes);

  // 3. Packets can also be constructed and sent manually
  auto packet = libreach::Packet(libreach::PacketId::POSITION, joint_a, bytes);
  driver.send_packet(packet);

  // Let the driver run indefinitely
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
