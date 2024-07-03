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
#include "libreach/udp_driver.hpp"

/// This example provides a minimal demonstration of how to use the serial driver to communicate with a Bravo 7.
auto main() -> int
{
  // Below are the default network connection parameters for the Bravo 7
  const std::string ip_address = "192.168.2.3";
  const std::uint16_t port = 6789;

  // Create a new UDP driver for the Bravo 7
  libreach::UdpDriver driver(ip_address, port);

  // Register a callback for the POSITION packet
  driver.register_callback(libreach::PacketId::POSITION, [](const libreach::Packet & packet) {
    std::cout << "Received POSITION packet with value: " << libreach::deserialize<float>(packet) << "\n";
  });

  // Request POSITION data at 10 Hz for joint A
  driver.request_at_rate(
    libreach::PacketId::POSITION,
    static_cast<std::uint8_t>(libreach::Bravo7DeviceId::JOINT_A),
    std::chrono::milliseconds(100));

  // Let the driver run indefinitely
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
