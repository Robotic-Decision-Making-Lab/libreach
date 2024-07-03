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

#include <cstdint>

namespace libreach
{

enum class Alpha5DeviceId : std::uint8_t
{
  END_EFFECTOR = 0x01,
  JOINT_D = 0x02,
  JOINT_C = 0x03,
  JOINT_B = 0x04,
  JOINT_A = 0x05,
  ALL_JOINTS = 0xFF,
};

enum class Bravo7DeviceId : std::uint8_t
{
  END_EFFECTOR = 0x01,
  JOINT_F = 0x02,
  JOINT_E = 0x03,
  JOINT_D = 0x04,
  JOINT_C = 0x05,
  JOINT_B = 0x06,
  JOINT_A = 0x07,
  ALL_JOINTS = 0xFF,
  FORCE_TORQUE_SENSOR = 0x0D,
};

}  // namespace libreach
