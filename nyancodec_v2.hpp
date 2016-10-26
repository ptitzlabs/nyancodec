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

const array<string, 12> dtype_s = {
    "enum",           // enumerator
    "int",            // integer
    "unsigned int",   // unsigned integer
    "float",          // float
    "unsigned float", // unsigned float
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
  ui,     // unsigned integer
  f,      // float
  uf,     // unsigned float
  vf,     // variable floating point
  time,   // time
  string, // string
  oct,    // raw bytes
  spare,  // spare bits
  rep,    // repeat flag
};
// item types
const array<string, 5> itype_s = {
    "fixed",     // fixed length item
    "extended",  // extended length item
    "variable",  // variable length item
    "repeating", // repetitive item
    "composite", // composite item
    "aux"        // auxillary item
};
enum class itype {
  fix, // fixed length item
  ext, // extended length item
  var, // variable length item
  rep, // repetitive item
  com, // composite item
  aux  // auxillary item
};

class datafield_spec {
public:
  void from_json(const json &j); // load from json
  void to_json(json &j);         // append to existing json
  json to_json();                // create new json
  string name1;                  // short name
  string name2;                  // long name
  vector<string> enum_val1;      // enum options(if present)
  vector<string> enum_val2;      // enum option values(if present)
  uint8_t bit_begin;             // first bit
  uint8_t bit_end;               // last bit+1
  dtype data_type;               // data type
};

class subfield_spec : public vector<datafield_spec> {
public:
  void from_json(const json &j); // load from json
  void to_json(json &j);         // append to existing json
  json to_json();                // create new json
  uint8_t bit_length; // total subfield bit length
  uint8_t byte_length; // total subfield byte length
};
class item_spec : public vector<subfield_spec> {
public:
  void from_json(const json &j); // load from json
  void to_json(json &j);         // append to existing json
  json to_json();                // create new json
  ftype item_type;               // item type
};
class cat_spec : public vector<item_spec> {
public:
  void from_json_file(const string & cat_filename, const string & uap_filename); // parse from .json files
  void from_json(const json &j); // load from json
  void to_json(json &j);         // append to existing json
  json to_json();                // create new json
  map<size_t, string> uap; // map containing item indices IXXX/XXX in UAP order
};

class fspec : public vector<size_t> {
public:
  const vector<oct>::iterator fspec_begin; // reference to msg raw data
                                           // containing the start of this fspec
};

class datafield {
public:
  void parse(vector<oct>::iterator &it);
  void from_json(const json &j);    // load from json
  void to_json(json &j);            // append to existing json
  json to_json();                   // create new json
  ostringstream to_string_stream(); // output to printable string stream
  float to_float(); // output as float
  long to_int();      // output as integer
  string to_string(); // output as string
  void decode( const vector<oct>::iterator &subfield_begin); // decode and store the data
  union {
    char str[20];
    long i;
    float f;
  } data; // decoded datafield data
  const datafield_spec &dspec; // reference to relevant datafield_spec object
};

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
class msg : public vector<block> {
public:
  void from_bin_file(const string & filename);  // parse from binary file
  void from_json_file(const string & filename); // parse from .json file
  void parse(auto &stream);             // parse from istream
  void from_json(const json &j);        // parse from json
  void to_json(json &j);                // append to existing json
  json to_json();                       // create new json
  ostream to_binary_stream();           // output to binary stream
  ostringstream to_string_stream();     // output to printable string stream
  void decode();                        // decode and store the data
  void encode();         // fill up the msg_data block with octets
  vector<oct> msg_data;  // contains raw message bytes
  const cat_spec &cspec; // reference to relevant cat spec object
  size_t cat_id;         // cat id
  size_t msg_len;        // message length in bytes(octets)
  void print(); // print to terminal
  void print(const string & filename); // print to file
};
}

#endif // NYANCODEC_V2_HPP
