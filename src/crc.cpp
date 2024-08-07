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

#include "crc.hpp"

#include <cstddef>
#include <cstdint>

namespace libreach::protocol
{

namespace
{

const std::uint8_t INITIAL_VALUE = 0x00;

const std::uint8_t FINAL_XOR_VALUE = 0xFF;

const bool INPUT_REFLECTED = true;

const bool RESULT_REFLECTED = true;

/// CRC8 lookup table calculated using the following parameters:
///   - Polynomial: 0x4D
///   - Initial Value: 0x00
///   - Final XOR Value: 0xFF
///   - Input Reflected: True
///   - Result Reflected: True
///   - Use Reflected Lookup Table: False
const std::array<std::uint8_t, 256> CRC8_LOOKUP_TABLE = {
  0x00, 0x4D, 0x9A, 0xD7, 0x79, 0x34, 0xE3, 0xAE, 0xF2, 0xBF, 0x68, 0x25, 0x8B, 0xC6, 0x11, 0x5C, 0xA9, 0xE4, 0x33,
  0x7E, 0xD0, 0x9D, 0x4A, 0x07, 0x5B, 0x16, 0xC1, 0x8C, 0x22, 0x6F, 0xB8, 0xF5, 0x1F, 0x52, 0x85, 0xC8, 0x66, 0x2B,
  0xFC, 0xB1, 0xED, 0xA0, 0x77, 0x3A, 0x94, 0xD9, 0x0E, 0x43, 0xB6, 0xFB, 0x2C, 0x61, 0xCF, 0x82, 0x55, 0x18, 0x44,
  0x09, 0xDE, 0x93, 0x3D, 0x70, 0xA7, 0xEA, 0x3E, 0x73, 0xA4, 0xE9, 0x47, 0x0A, 0xDD, 0x90, 0xCC, 0x81, 0x56, 0x1B,
  0xB5, 0xF8, 0x2F, 0x62, 0x97, 0xDA, 0x0D, 0x40, 0xEE, 0xA3, 0x74, 0x39, 0x65, 0x28, 0xFF, 0xB2, 0x1C, 0x51, 0x86,
  0xCB, 0x21, 0x6C, 0xBB, 0xF6, 0x58, 0x15, 0xC2, 0x8F, 0xD3, 0x9E, 0x49, 0x04, 0xAA, 0xE7, 0x30, 0x7D, 0x88, 0xC5,
  0x12, 0x5F, 0xF1, 0xBC, 0x6B, 0x26, 0x7A, 0x37, 0xE0, 0xAD, 0x03, 0x4E, 0x99, 0xD4, 0x7C, 0x31, 0xE6, 0xAB, 0x05,
  0x48, 0x9F, 0xD2, 0x8E, 0xC3, 0x14, 0x59, 0xF7, 0xBA, 0x6D, 0x20, 0xD5, 0x98, 0x4F, 0x02, 0xAC, 0xE1, 0x36, 0x7B,
  0x27, 0x6A, 0xBD, 0xF0, 0x5E, 0x13, 0xC4, 0x89, 0x63, 0x2E, 0xF9, 0xB4, 0x1A, 0x57, 0x80, 0xCD, 0x91, 0xDC, 0x0B,
  0x46, 0xE8, 0xA5, 0x72, 0x3F, 0xCA, 0x87, 0x50, 0x1D, 0xB3, 0xFE, 0x29, 0x64, 0x38, 0x75, 0xA2, 0xEF, 0x41, 0x0C,
  0xDB, 0x96, 0x42, 0x0F, 0xD8, 0x95, 0x3B, 0x76, 0xA1, 0xEC, 0xB0, 0xFD, 0x2A, 0x67, 0xC9, 0x84, 0x53, 0x1E, 0xEB,
  0xA6, 0x71, 0x3C, 0x92, 0xDF, 0x08, 0x45, 0x19, 0x54, 0x83, 0xCE, 0x60, 0x2D, 0xFA, 0xB7, 0x5D, 0x10, 0xC7, 0x8A,
  0x24, 0x69, 0xBE, 0xF3, 0xAF, 0xE2, 0x35, 0x78, 0xD6, 0x9B, 0x4C, 0x01, 0xF4, 0xB9, 0x6E, 0x23, 0x8D, 0xC0, 0x17,
  0x5A, 0x06, 0x4B, 0x9C, 0xD1, 0x7F, 0x32, 0xE5, 0xA8};

/// Reflect data about the center bit.
/// Inspired by the following source:
///  - https://barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code
auto reflect(uint64_t data, std::size_t size) -> std::uint8_t
{
  uint64_t reflection = 0x00000000;

  for (std::size_t bit = 0; bit < size; ++bit) {
    if (data & 0x01) {  // NOLINT
      reflection |= (1 << ((size - 1) - bit));
    }

    data = (data >> 1);
  }

  return static_cast<std::uint8_t>(reflection);
}

/// Calculate the CRC value for a message using the CRC8 algorithm.
auto calculate_crc8(
  const std::vector<std::uint8_t> & data,
  std::uint8_t initial_value,
  std::uint8_t final_xor_value,
  bool input_reflected,
  bool result_reflected,
  const std::array<std::uint8_t, 256> & lookup_table) -> std::uint8_t
{
  std::uint8_t crc = initial_value;
  std::uint8_t value;

  const int width = (8 * sizeof(crc));

  for (const std::uint8_t byte : data) {
    if (input_reflected) {
      value = reflect(byte, 8);
    } else {
      value = byte;
    }

    value ^= crc >> (width - 8);
    crc = lookup_table[value] ^ (crc << 8);
  }

  if (result_reflected) {
    crc = reflect(crc, width);
  }

  return crc ^ final_xor_value;
}

}  // namespace

auto calculate_crc(const std::vector<std::uint8_t> & data) -> std::uint8_t
{
  return calculate_crc8(data, INITIAL_VALUE, FINAL_XOR_VALUE, INPUT_REFLECTED, RESULT_REFLECTED, CRC8_LOOKUP_TABLE);
}

}  // namespace libreach::protocol
