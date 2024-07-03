// Copyright 2023, Evan Palmer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "cobs.hpp"

#include <cstdint>
#include <stdexcept>

namespace libreach::protocol
{

auto encode_cobs(const std::vector<std::uint8_t> & data) -> std::vector<std::uint8_t>
{
  std::vector<std::uint8_t> encoded_data = {0x00};

  int block_start = 0;
  int current_block_size = 0;

  for (const std::uint8_t it : data) {
    if (it == 0x00) {
      encoded_data[block_start] = static_cast<std::uint8_t>(current_block_size + 1);

      encoded_data.push_back(0x00);

      block_start = encoded_data.size() - 1;
      current_block_size = 0;
    } else {
      encoded_data.push_back(it);
      current_block_size++;

      if (current_block_size >= 254) {
        encoded_data[block_start] = static_cast<std::uint8_t>(current_block_size + 1);

        encoded_data.push_back(0x00);

        block_start = encoded_data.size() - 1;
        current_block_size = 0;
      }
    }
  }

  encoded_data[block_start] = static_cast<std::uint8_t>(current_block_size + 1);
  encoded_data.push_back(0x00);

  return encoded_data;
}

auto decode_cobs(const std::vector<std::uint8_t> & data) -> std::vector<std::uint8_t>
{
  std::vector<std::uint8_t> decoded_data;
  std::vector<std::uint8_t>::size_type encoded_data_pos = 0;

  while (encoded_data_pos < data.size()) {
    const std::size_t block_size = data[encoded_data_pos] - 1;
    encoded_data_pos++;

    for (std::size_t i = 0; i < block_size; ++i) {
      const std::uint8_t byte = data[encoded_data_pos];

      if (byte == 0x00) {
        throw std::runtime_error("Failed to decode the encoded data.");
      }

      decoded_data.push_back(data[encoded_data_pos]);
      encoded_data_pos++;
    }

    if (data[encoded_data_pos] == 0x00) {
      break;
    }

    if (block_size < 0xFE) {
      decoded_data.push_back(0x00);
    }
  }

  return decoded_data;
}

}  // namespace libreach::protocol
