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

#include <iostream>
#include <string>

#include "libreach/device_id.hpp"
#include "libreach/packet.hpp"
#include "libreach/packet_id.hpp"
#include "libreach/serial_driver.hpp"

/// This example demonstrates ways that packets can be requested from a robot using libreach.
auto main() -> int
{
  const std::string serial_port = "/dev/ttyUSB0";
  libreach::SerialDriver driver(serial_port);

  driver.register_callback(libreach::PacketId::POSITION, [](const libreach::Packet & packet) {
    std::cout << "Received POSITION packet with value: " << libreach::deserialize<float>(packet) << "\n";
  });

  driver.register_callback(libreach::PacketId::VELOCITY, [](const libreach::Packet & packet) {
    std::cout << "Received VELOCITY packet with value: " << libreach::deserialize<float>(packet) << "\n";
  });

  driver.register_callback(libreach::PacketId::CURRENT, [](const libreach::Packet & packet) {
    std::cout << "Received CURRENT packet with value: " << libreach::deserialize<float>(packet) << "\n";
  });

  driver.register_callback(libreach::PacketId::RELATIVE_POSITION, [](const libreach::Packet & packet) {
    std::cout << "Received RELATIVE_POSITION packet with value: " << libreach::deserialize<float>(packet) << "\n";
  });

  // Request a POSITION packet from joint A at a rate of 10 Hz (every 100 ms)
  driver.request_at_rate(
    libreach::PacketId::POSITION,
    static_cast<std::uint8_t>(libreach::Alpha5DeviceId::JOINT_A),
    std::chrono::milliseconds(100));

  // Request VELOCITY and CURRENT packets from joint B at a rate of 5 Hz (every 200 ms)
  driver.request_at_rate(
    {libreach::PacketId::VELOCITY, libreach::PacketId::CURRENT},
    static_cast<std::uint8_t>(libreach::Alpha5DeviceId::JOINT_B),
    std::chrono::milliseconds(200));

  while (true) {
    // Send a one-time request for the RELATIVE_POSITION packet from joint C
    driver.request(libreach::PacketId::RELATIVE_POSITION, static_cast<std::uint8_t>(libreach::Alpha5DeviceId::JOINT_C));

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}