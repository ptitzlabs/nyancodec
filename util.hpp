#ifndef UTIL_HPP
#define UTIL_HPP
#include <array>
#include <limits>
#include <bitset>
#include "include/console_color.hpp"

#define DEBUG
namespace util {
using namespace std;
const array<unsigned char, 8> bit_mask = {
    0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1}; // individual bit mask

const array<unsigned char, 9> multi_bit_mask = {
    0b00000000, 0b00000001, 0b00000011, 0b00000111, 0b00001111,
    0b00011111, 0b00111111, 0b01111111, 0b11111111,
};

const array<char,58> b6_char = {
    ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

const string nyan_s =
"░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n"
"░░░░░░░░░░▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄░░░░░░░░░\n"
"░░░░░░░░▄▀░░░░░░░░░░░░▄░░░░░░░▀▄░░░░░░░\n"
"░░░░░░░░█░░▄░░░░▄░░░░░░░░░░░░░░█░░░░░░░\n"
"░░░░░░░░█░░░░░░░░░░░░▄█▄▄░░▄░░░█░▄▄▄░░░\n"
"░▄▄▄▄▄░░█░░░░░░▀░░░░▀█░░▀▄░░░░░█▀▀░██░░\n"
"░██▄▀██▄█░░░▄░░░░░░░██░░░░▀▀▀▀▀░░░░██░░\n"
"░░▀██▄▀██░░░░░░░░▀░██▀░░░░░░░░░░░░░▀██░\n"
"░░░░▀████░▀░░░░▄░░░██░░░▄█░░░░▄░▄█░░██░\n"
"░░░░░░░▀█░░░░▄░░░░░██░░░░▄░░░▄░░▄░░░██░\n"
"░░░░░░░▄█▄░░░░░░░░░░░▀▄░░▀▀▀▀▀▀▀▀░░▄▀░░\n"
"░░░░░░█▀▀█████████▀▀▀▀████████████▀░░░░\n"
"░░░░░░████▀░░███▀░░░░░░▀███░░▀██▀░░░░░░\n"
"░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n";


inline string int_to_time(long t, int res = 7) {
  ostringstream oss;
  if (t >> res > 3600) {
    uint8_t hr = (t >> res) / 3600;
    uint8_t min = ((t >> res) / 60) % 60;
    float sec = float(t) / (2 << (res - 1)) - hr * 3600 - min * 60;
    oss << int(hr) << ":" << int(min) << ":" << internal << setfill('0')
        << setprecision(5) << sec;
  } else {
    oss << static_cast<float>(t * 1.0 / (1 << res));
  }
  return oss.str();
}
inline string hex_to_str(char h) {
  ostringstream oss;
  oss << internal << setfill('0') << hex << setw(2) << int(h);
  return oss.str();
}
inline string hex_to_str(long h, uint8_t n, bool spaced = false) {
  ostringstream oss;
  oss << internal << setfill('0');
  if (spaced) {
    if (n > 0) {
      for (auto i = n - 1; i > 0; i--) {
        oss << hex << setw(2) << int((h & (0xff << (i << 3))) >> (i >> 3))
            << " ";
      }
      oss << hex << setw(2) << int((h & 0xff));
    } else {
      oss << hex << setw(2) << int((h & 0xff000000) >> 24) << " ";
      oss << hex << setw(2) << int((h & 0xff0000) >> 16) << " ";
      oss << hex << setw(2) << int((h & 0xff00) >> 8) << " ";
      oss << hex << setw(2) << int((h & 0xff));
    }
  } else {
    if (n > 0) {
      oss << hex << setw(n * 2) << int(h);
    } else {
      oss << hex << int(h);
    }
  }
  return oss.str();
}

inline string bin_to_str(long h, const uint8_t n) {
  switch (n) {
  case 1:
    return bitset<1>(h).to_string();
  case 2:
    return bitset<2>(h).to_string();
  case 3:
    return bitset<3>(h).to_string();
  case 4:
    return bitset<4>(h).to_string();
  case 8:
    return bitset<8>(h).to_string();
  case 16:
    return bitset<16>(h).to_string();
  case 24:
    return bitset<26>(h).to_string();
  case 32:
    return bitset<32>(h).to_string();
  }
  return 0;
}


void oct_to_long(const auto &hex_arr, uint16_t byte_loc, uint16_t bit_loc,
               uint8_t len, long &dest, const bool &sign = false) {
  if (len == 1) {
    // grab a single bit
    dest = (hex_arr[byte_loc] >> (7 - bit_loc)) & 0b1;
  } else {
    if (bit_loc == 0 && len % 8 == 0) {
      // grab a standard sized variable
      switch (len) {
      case 8:
        dest = hex_arr[byte_loc];
        break;
      case 16:
        dest = (hex_arr[byte_loc] << 8) + hex_arr[byte_loc + 1];
        break;
      case 24:
        dest = (hex_arr[byte_loc] << 16) + (hex_arr[byte_loc + 1] << 8) +
               hex_arr[byte_loc + 2];
        break;
      case 32:
        dest = (hex_arr[byte_loc] << 24) + (hex_arr[byte_loc + 1] << 16) +
               (hex_arr[byte_loc + 2] << 8) + hex_arr[byte_loc + 3];
        break;
      }
    } else {
      // grab a random sized variable
      // first check if the variable actually fits
      uint16_t bit_end = bit_loc + len;
      if (bit_end < 8) {
        // if it's still within the octet, grab it
        dest = (hex_arr[byte_loc] >> (8 - bit_end)) & multi_bit_mask[len];
      } else {
        // the variable starts in the middle of octet and extends into the next
        // one
        // make an iterator for current octet
        auto arr_it = hex_arr + byte_loc;
        // start filling up the destination variable
        // first grab the rest of the octet
        len -= (8 - bit_loc);
        dest = *arr_it & multi_bit_mask[8 - bit_loc];
        // iterate if the rest of the item spans several bytes
        while (len > 8) {
          // iterate the byte
          arr_it++;
          // shift the destination 8 bits and add current octet
          dest = (dest << 8) + *arr_it;
          // substract 8 bits from the length
          len -= 8;
        }
        // if all the bits are home or if the rest is only a few bits then
        // finish
        if (len > 0) {
          // iterate the byte
          arr_it++;
          // shift the destination 8 bits and add current octet
          dest = (dest << len) + (*arr_it >> (8 - len));
        }
      }
      // attach a sign if needed (fill the space with ones)
    }
    if (sign && (dest & (1 << (len - 1)))) {
      dest += numeric_limits<unsigned long>::max() << len;
    }
  }
}
void grab_string(const auto &hex_arr, const uint8_t &string_bit_len, auto *dest,
                 const uint8_t &char_len) {
  uint16_t bit_counter = 0;
  long char_buffer;
  do {
    oct_to_long(hex_arr, bit_counter>>3, bit_counter%8, char_len, char_buffer);
    bit_counter += char_len;
    *dest = b6_char[char_buffer];
    dest++;
  } while (bit_counter != string_bit_len);
}
void hex_to_text_stream(const auto & it_begin, const auto & it_end,auto & stream){
  uint8_t format_counter = 0;
  stream<<"0x ";
  for(auto it = it_begin; it != it_end; it++){
      if((format_counter)%4==0){
          if(format_counter%16!=0){ stream<<" ";
          }
          else{
              if(it != it_begin){
                  stream<<endl;
                  if(it!=it_end-1)
                      stream<<"0x ";
              }
          }
      }
      format_counter++;
     stream<<internal<<setfill('0')<<hex<<setw(2)<<int(*it)<<" ";
  }
  stream<<dec<<flush;
//  if((format_counter-1)%16==0) cout<<flush;
//  else stream<<endl;
}
}

#endif // UTIL_HPP
