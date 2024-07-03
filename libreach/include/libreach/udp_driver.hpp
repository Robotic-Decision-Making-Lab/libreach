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

#include <chrono>
#include <cstddef>
#include <string>

#include "libreach/driver.hpp"
#include "libreach/udp_client.hpp"

namespace libreach
{

class UdpDriver : public ReachDriver
{
public:
  /// Create a new serial driver using:
  ///   - an IP address for communication (e.g., "192.168.2.3"),
  ///   - a port number for communication (e.g., 12345),
  ///   - a queue size for storing incoming packets,
  ///   - a number of worker threads for processing incoming packets,
  ///   - a session timeout for heartbeat monitoring.
  explicit UdpDriver(
    const std::string & addr,
    const std::uint16_t port,
    std::size_t q_size = 100,
    std::size_t n_workers = 1,
    std::chrono::seconds session_timeout = std::chrono::seconds(3));

  ~UdpDriver() = default;
};

}  // namespace libreach
