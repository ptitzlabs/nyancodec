#ifndef NYANCODEC_V2_HPP
#define NYANCODEC_V2_HPP
#include "include/json.hpp"
#include "include/range.hpp"
#include "util.hpp"
#include <iostream>
#include <bitset>
#include <string>
#include <iterator>
#include <tuple>
#include <utility>
#include <map>
#include <unordered_map>
#include <bitset>
#include <ctime>
#include <fstream>
namespace nyancodec {
using namespace nlohmann;
using namespace std;
using util::lang::indices;
using util::grab_bits;
using util::bit_mask;
using util::multi_bit_mask;
using util::int_to_time;
using util::hex_to_str;
using util::bin_to_str;
using util::grab_string;
using util::b6_char;
typedef unsigned char oct;

const array<string, 9> dtype_s = {
    "enum",           // enumerator
    "int",            // integer
    "float",          // float
    "variable float", // variable floating point
    "time",           // time
    "string",         // string
    "oct",            // raw bytes
    "spare",          // spare bits
    "rep",            // repeat flag
};

enum class dtype {
  num,    // enumerator
  i,      // integer
  f,      // float
  vf,     // variable floating point
  time,   // time
  string, // string
  oct,    // raw bytes
  spare,  // spare bits
  rep,    // repeat flag
};

const map<string, dtype> dtype_map{
    {"enum", dtype::num},
    {"int", dtype::i},
    {"float", dtype::f},
    {"lsb-selector", dtype::vf},
    {"time", dtype::time},
    {"string", dtype::string},
    {"oct", dtype::num},
    {"spare", dtype::spare},
    {"rep", dtype::rep},
};
// item types
enum class itype {
  fix, // fixed length item
  ext, // extended length item
  var, // variable length item
  rep, // repetitive item
  com, // composite item
  aux  // auxillary item
};
const map<string, itype> itype_map = {
    {"fixed", itype::fix},     // fixed length item
    {"extended", itype::ext},  // extended length item
    {"variable", itype::var},  // variable length item
    {"repeating", itype::rep}, // repetitive item
    {"composite", itype::com}, // composite item
    {"aux", itype::aux},       // auxillary item
};

//// CAT classes
// data field
class datafield_spec {
public:
  void from_json(const json &j) { // load from json
    name1 = j["name1"];
    name2 = j["name2"];
    bit_begin = j["bit begin"];
    bit_end = j["bit end"];
    data_type = dtype_map[j["type"]];
    switch (data_type) {
    case dtype::num:
      for (auto &a : j["options"]) {
        enum_val1.push_back(a[0]);
        enum_val2.push_back(a[1]);
      }
      break;
    case dtype::f:
      lsb = j["lsb"];
      unit = j["unit"];
      break;
    case dtype::vf:
      for (auto &a : j["options"]) {
        enum_val1.push_back(a[0]);
        enum_val3.push_back(a[1]);
        enum_val1.push_back(a[2]);
      }
      break;
    case dtype::time:
      lsb = j["lsb"];
      unit = j["unit"];
      break;
    case dtype::string:
      char_size = j["char size"];
      str_len = j["str len"];
      break;
    default:
      break;
    }
  }
  void to_json(json &j) { // append to existing json
    j["name1"] = name1;
    j["name2"] = name2;
    j["bit begin"] = bit_begin;
    j["bit end"] = bit_end;
    dtype_map[j["type"]] = data_type;
    switch (data_type) {
    case dtype::num:
      j["options"].resize(enum_val1.size());
      for (auto i : indices(enum_val1)) {
        j["options"][i] = tuple<string, string>(enum_val1[i], enum_val2[i]);
      }
      break;
    case dtype::f:
      j["lsb"] = lsb;
      j["unit"] = unit;
      break;
    case dtype::vf:
      j["options"].resize(enum_val1.size());
      for (auto i : indices(enum_val1)) {
        j["options"][i] = tuple<string, double, string>(
            enum_val1[i], enum_val3[i], enum_val2[i]);
      }
      break;
    case dtype::time:
      j["lsb"] = lsb;
      j["unit"] = unit;
      break;
    case dtype::string:
      j["char size"] = char_size;
      j["str len"] = str_len;
      break;
    default:
      break;
    }
  }
  json to_json() { // create new json
    json j;
    to_json(j);
    return j;
  }
  string name1;             // short name
  string name2;             // long name
  vector<string> enum_val1; // enum options(if present)
  vector<string> enum_val2; // enum option values(if present)
  vector<float> enum_val3;  // enum option values(if present)
  uint8_t bit_begin;        // first bit
  uint8_t bit_end;          // last bit+1
  dtype data_type;          // data type
  float lsb;                // least significant bit
  string unit;              // unit
  uint8_t char_size;
  uint8_t str_len;
};

// item subfield
class subfield_spec : public vector<datafield_spec> {
public:
  void from_json(const json &j) { // load from json
      resize(j["data"].size());
      for(auto i: indices(*this)){
          (*this)[i].from_json(j["data"][i]);
      }
  }
  void to_json(json &j) {// append to existing json
  }
  json to_json();        // create new json
  uint8_t bit_length;    // total subfield bit length
  uint8_t byte_length;   // total subfield byte length
};

// item
class item_spec : public vector<subfield_spec> {
public:
  void from_json(const json &j); // load from json
  void to_json(json &j);         // append to existing json
  json to_json();                // create new json
  ftype item_type;               // item type
};

// full cat
class cat_spec : public vector<item_spec> {
public:
  void from_json_file(const string &cat_filename,  // cat filename
                      const string &uap_filename); // uap filename
  void from_json(const json &j);                   // load from json
  void to_json(json &j);                           // append to existing json
  json to_json();                                  // create new json
  map<size_t, string> uap; // map containing item indices IXXX/XXX in UAP order
};

// fspec
class fspec : public vector<size_t> {
public:
  const vector<oct>::iterator fspec_begin; // reference to msg raw data
                                           // containing the start of this fspec
};

// data field containing decoded data
class datafield {
public:
  void parse(vector<oct>::iterator &it);
  void from_json(const json &j);    // load from json
  void to_json(json &j);            // append to existing json
  json to_json();                   // create new json
  ostringstream to_string_stream(); // output to printable string stream
  float to_float();                 // output as float
  long to_int();                    // output as integer
  string to_string();               // output as string
  void decode(
      const vector<oct>::iterator &subfield_begin); // decode and store the data
  union {
    char str[20];
    long i;
    float f;
  } data;                      // decoded datafield data
  const datafield_spec &dspec; // reference to relevant datafield_spec object
};

// subfield containing a vector of decoded data
class subfield : public vector<datafield> {
public:
  void parse(vector<oct>::iterator &it); // parse from msg data
  void from_json(const json &j);         // load from json
  void to_json(json &j);                 // append to existing json
  json to_json();                        // create new json
  ostream to_binary_stream();            // output to binary stream
  ostringstream to_string_stream();      // output to printable string stream
  void decode();                         // decode and store the data
  void encode(vector<oct>::iterator &msg_it); // fill up the msg_data block with
                                              // octets, corresponding to this
                                              // subfield contents
  const vector<oct>::iterator subfield_begin; // reference to msg raw data
  // containing the start of this subfield
  const subfield_spec &sspec; // reference to relevant subfield spec object
};

// item, containing subfields with decoded data
class item : public vector<subfield> {
public:
  void parse(vector<oct>::iterator &it); // parse from msg data
  void from_json(const json &j);         // parse from json
  void to_json(json &j);                 // append to existing json
  json to_json();                        // create new json
  ostream to_binary_stream();            // output to binary stream
  ostringstream to_string_stream();      // output to printable string stream
  void decode();                         // decode and store the data
  void encode(vector<oct>::iterator &msg_it); // fill up the msg_data block with
                                              // octets, corresponding to this
                                              // item contents
  const vector<oct>::iterator item_begin;     // reference to msg raw data
  // containing the start of this item
  fspec fs;               // fspec(only for compound items)
  const item_spec &ispec; // reference to relevant item spec object
};

// block containing items with decoded data
class block : public vector<item> {
public:
  void parse(vector<oct>::iterator &it); // parse from msg data
  void from_json(const json &j);         // parse from json
  void to_json(json &j);                 // append to existing json
  json to_json();                        // create new json
  ostream to_binary_stream();            // output to binary stream
  ostringstream to_string_stream();      // output to printable string stream
  void decode();                         // decode and store the data
  void encode(vector<oct>::iterator &msg_it); // fill up the msg_data block with
                                              // octets, corresponding to this
                                              // block contents
  const vector<oct>::iterator block_begin;    // reference to msg raw data
  // containing the start of this block
  fspec fs;              // block fspec
  const cat_spec &cspec; // reference to relevant cat spec object
};

// message, containing various blocks
class msg : public vector<block> {
public:
  void from_bin_file(const string &filename);  // parse from binary file
  void from_json_file(const string &filename); // parse from .json file
  void parse(auto &stream);                    // parse from istream
  void from_json(const json &j);               // parse from json
  void to_json(json &j);                       // append to existing json
  json to_json();                              // create new json
  ostream to_binary_stream();                  // output to binary stream
  ostringstream to_string_stream();   // output to printable string stream
  void decode();                      // decode and store the data
  void encode();                      // fill up the msg_data block with octets
  vector<oct> msg_data;               // contains raw message bytes
  const cat_spec &cspec;              // reference to relevant cat spec object
  size_t cat_id;                      // cat id
  size_t msg_len;                     // message length in bytes(octets)
  void print();                       // print to terminal
  void print(const string &filename); // print to file
};
}

#endif // NYANCODEC_V2_HPP
