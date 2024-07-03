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
#include <functional>
#include <string>

#include "libreach/client.hpp"

namespace libreach::protocol
{

class SerialClient : public Client
{
public:
  /// Create a new serial client given a serial port, packet callback, and a session timeout.
  explicit SerialClient(
    const std::string & port,
    std::function<void(const std::vector<Packet> &)> && callback,
    std::chrono::seconds session_timeout);

  ~SerialClient() override;

private:
  /// Read n bytes from the serial port into a buffer.
  auto read_bytes(std::deque<std::uint8_t> & buffer, std::size_t n_bytes) const -> ssize_t override;

  /// Write data to the serial port.
  auto write_to_connection(const std::vector<std::uint8_t> & data) const -> ssize_t override;

  int handle_;
};

}  // namespace libreach::protocol
