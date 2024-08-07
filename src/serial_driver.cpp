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

#include "libreach/serial_driver.hpp"

#include <chrono>
#include <memory>

namespace libreach
{

SerialDriver::SerialDriver(
  const std::string & port,
  std::size_t q_size,
  std::size_t n_workers,
  std::chrono::seconds session_timeout)
: ReachDriver(
    std::make_unique<protocol::SerialClient>(
      port,
      [this](const std::vector<Packet> & packets) { receive_packets(packets); },
      session_timeout),
    q_size,
    n_workers)
{
}

}  // namespace libreach
