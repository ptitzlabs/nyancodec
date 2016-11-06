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
#include "dbug.h"
namespace nyancodec {
using namespace nlohmann;
using namespace std;
using util::lang::indices;
using util::oct_to_long;
using util::bit_mask;
using util::multi_bit_mask;
using util::int_to_time;
using util::hex_to_str;
using util::bin_to_str;
using util::grab_string;
using util::b6_char;
using util::err;
using util::print_err;

using util::hex_to_text_stream;

typedef unsigned char oct;

const array<string, 10> dtype_s = {
    "enum",           // enumerator
    "int",            // integer
    "float",          // float
    "variable float", // variable floating point
    "time",           // time
    "string",         // string
    "char",           // character
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
  ch,     // character
  oct,    // raw bytes
  spare,  // spare bits
  rep,    // repeat flag
};

static const map<string, dtype> dtype_map{
    {"enum", dtype::num},
    {"int", dtype::i},
    {"float", dtype::f},
    {"lsb-selector", dtype::vf},
    {"time", dtype::time},
    {"string", dtype::string},
    {"char", dtype::ch},
    {"oct", dtype::oct},
    {"spare", dtype::spare},
    {"rep", dtype::rep},
};
// item types
enum class itype {
  fix,   // fixed length item
  ext,   // extended length item
  var,   // variable length item
  rep,   // repetitive item
  com,   // composite item
  aux,   // auxillary item
  ref,   // reserved expansion field
  sp,    // special purpose indicator
  blank, // unspecified item
  spare  // spare item
};
const map<string, itype> itype_map = {
    {"fixed", itype::fix},      // fixed length item
    {"extendible", itype::ext}, // extended length item
    {"variable", itype::var},   // variable length item
    {"repetitive", itype::rep}, // repetitive item
    {"compound", itype::com},   // composite item
    {"aux", itype::aux},        // auxillary item
    {"ref", itype::ref},        // reserved expansion field
    {"sp", itype::sp},          // special purpose indicator
    {"blank", itype::blank},    // unspecified item
    {"spare", itype::spare},    // spare item
};

//// CAT classes
// data field
#ifdef BLE
#endif
class datafield_spec {
public:
  datafield_spec(json &j, json &item_name, uint8_t &subfield_id,
                 uint8_t datafield_id, uint16_t &subfield_bit_len)
      : datafield(j), item_name(item_name), subfield_id(subfield_id),
        datafield_id(datafield_id) {
    if (j.find("bit begin") == j.end()) {
      cout << WARNING << static_cast<const string &>(item_name) << " subfield "
           << int(subfield_id) + 1 << " datafield " << int(datafield_id) + 1
           << ": Data first bit undefined" << endl;
      bit_begin = 0;
    } else {
      bit_begin = subfield_bit_len - static_cast<const int &>(j["bit begin"]);
    }
    if (j.find("bit begin") == j.end()) {
      cout << WARNING << static_cast<const string &>(item_name) << " subfield "
           << int(subfield_id) + 1 << " datafield " << int(datafield_id) + 1
           << ": Data last bit undefined" << endl;
      bit_end = 0;
    } else {
      bit_end = subfield_bit_len - static_cast<const int &>(j["bit end"]);
    }
    bit_len = bit_end - bit_begin;
    byte_len = bit_len >> 8;
    if (bit_len <= 0) {
      cout << WARNING << static_cast<const string &>(item_name) << " subfield "
           << int(subfield_id) + 1 << " datafield " << int(datafield_id) + 1
           << ": illegal bit length " << int(bit_len) << endl;
    }
    byte_loc = bit_begin >> 3;
    bit_loc = bit_begin % 8;
    if (j.find("type") == j.end()) {
      cout << WARNING << static_cast<const string &>(item_name) << " subfield "
           << int(subfield_id) + 1 << " datafield " << int(datafield_id) + 1
           << ": Data has no type" << endl;
      data_type = dtype::i;
    } else {
      if (dtype_map.find(j["type"]) == dtype_map.end()) {
        cout << WARNING << static_cast<const string &>(item_name)
             << " subfield " << int(subfield_id) + 1 << " datafield "
             << int(datafield_id) + 1 << ": Unknown data type " << j["type"]
             << endl;
        data_type = dtype::oct;
      } else {
        data_type = dtype_map.at(j["type"]);
      }
    }
    switch (data_type) {
    case dtype::f:
      if (j.find("lsb") == j.end()) {
        cout << WARNING << static_cast<const string &>(item_name)
             << " subfield " << int(subfield_id) + 1 << " datafield "
             << int(datafield_id) + 1
             << ": floating point lsb not specified, assuming 1.0" << endl;
        lsb = 1.0;
      } else {
        lsb = j["lsb"];
      }
      if (j.find("signed") == j.end()) {
        cout << WARNING << static_cast<const string &>(item_name)
             << " subfield " << int(subfield_id) + 1 << " datafield "
             << int(datafield_id) + 1
             << ": floating point complement not specified, assuming signed"
             << endl;
        sign = true;
      } else {
        sign = j["signed"];
      }
      break;
    case dtype::time:
      if (j.find("lsb") == j.end()) {
        cout << WARNING << static_cast<const string &>(item_name)
             << " subfield " << int(subfield_id) + 1 << " datafield "
             << int(datafield_id) + 1
             << ": time lsb not specified, assuming 1.0" << endl;
        lsb = 1.0;
      } else {
        lsb = j["lsb"];
      }
      break;
    default:
      sign = false;
      break;
    }
  }
  datafield_spec &operator=(const datafield_spec &rhs) {
    datafield = rhs.datafield;
    item_name = rhs.item_name;
    subfield_id = rhs.subfield_id;
    datafield_id = rhs.datafield_id;
    bit_begin = rhs.bit_begin;
    bit_end = rhs.bit_end;
    data_type = rhs.data_type;
    return *this;
  }

  json &datafield;
  json &item_name;
  uint8_t &subfield_id;
  uint8_t datafield_id;
  uint16_t bit_begin;
  uint16_t bit_end;
  uint16_t bit_len;
  uint8_t byte_loc;
  uint8_t bit_loc;
  uint8_t byte_len;
  dtype data_type;

  // type-specific fields
  float lsb;
  bool sign;
};

// item subfield
// class subfield_spec{
class subfield_spec : public vector<datafield_spec> {
public:
  subfield_spec(json &j, json &item_name, uint8_t subfield_id)
      : subfield(j), item_name(item_name), subfield_id(subfield_id) {
    if (j["data"].size() == 0) {
      cout << WARNING << static_cast<const string &>(item_name) << " subfield "
           << int(subfield_id) + 1 << ": Subfield data not specified" << endl;
    } else {
      if (j["data"][0].find("bit begin") == j["data"][0].end()) {
        cout << WARNING << static_cast<const string &>(item_name)
             << " subfield " << int(subfield_id) + 1
             << ": Cannot deduce subfield size" << endl;
      } else {
        if (j["data"][0]["type"] != "rep") {
          bit_len = j["data"][0]["bit begin"];
        } else {
          // the item is probably repetitive
            cout<<j["data"].dump(1);
          bit_len = j["data"][1]["bit begin"];
        }
        byte_len = bit_len >> 3;
      }
      reserve(j["data"].size());
      int id_counter = 0;
      for (json &d : j["data"]) {
        emplace(end(), d, item_name, this->subfield_id, id_counter++, bit_len);
      }
    }
  }
  subfield_spec &operator=(const subfield_spec &rhs) {
    assign(rhs.begin(), rhs.end());
    subfield = rhs.subfield;
    bit_len = rhs.bit_len;
    byte_len = rhs.byte_len;
    return *this;
  }
  json &subfield;
  json &item_name;
  uint8_t subfield_id;
  uint16_t bit_len;
  uint8_t byte_len;
};

class item_spec : public vector<subfield_spec> {
  // item
  // class item_spec {
public:
  item_spec(json &j, json &name) : item(j), name(name) {
        cout << STATUS << static_cast<const string &>(name)
             << ": loading item" << endl;
    if (item.find("format") == item.end()) {
      cout << WARNING << static_cast<const string &>(name)
           << ": Item format not specified" << endl;
    } else {
      if (itype_map.find(j["format"]) == itype_map.end()) {
        cout << WARNING << static_cast<const string &>(name)
             << ": Undefined item format " << j["format"] << endl;
      } else {
        item_type = itype_map.at(j["format"]);
        if (j["subfields"].size() == 0) {
          if (item_type != itype::ref && item_type != itype::sp &&
              item_type != itype::spare)
            cout << WARNING << static_cast<const string &>(name)
                 << ": Item has no subfields" << endl;
        } else {
          reserve(j["subfields"].size());
          uint8_t id_counter = 0;
          for (json &sub : j["subfields"]) {
            emplace(end(), sub, this->name, id_counter++);
            switch (item_type) {
            case itype::fix:
              byte_len = 0;
              for (subfield_spec &sub : *this) {
                byte_len += sub.byte_len;
              }
              if (j.find("length") == j.end()) {
                cout << WARNING << static_cast<const string &>(name)
                     << " fixed-type length not set" << endl;
              } else {
                if (byte_len != j["length"]) {
                  cout << WARNING << static_cast<const string &>(name)
                       << " fixed-type length subfields does not match" << endl;
                }
              }
              break;
            default:
              break;
            }
          }
        }
      }
    }
  }
  item_spec &operator=(const item_spec &rhs) {
    assign(rhs.begin(), rhs.end());
    this->item = rhs.item;
    this->item_type = rhs.item_type;
    return *this;
  }
  json &item;
  itype item_type; // item type
  json &name;
  uint8_t byte_len;
};

// full cat
// class cat_spec {
class cat_spec : public vector<item_spec> {
public:
  cat_spec(const pair<json &, json &> &p)
      : cat(p.first), uap(p.second),
        name(static_cast<const string &>(cat["cat"])) {
    reserve(uap["uap"].size());
    for (auto &item : uap["uap"]) {
      if (item[0] != "spare") {
        if (cat.find(item[0]) == cat.end()) {
          cout << WARNING << name << ": Undefined item " << item[0];
          if (item[0] == "RE") {
            cout << ", assuming REF" << endl;
            cat[static_cast<const string &>(item[0])]["format"] = "ref";
          } else {
            if (item[0] == "SP") {
              cout << ", assuming SP" << endl;
              cat[static_cast<const string &>(item[0])]["format"] = "sp";
            } else {
              cout << ", assuming blank" << endl;
              cat[static_cast<const string &>(item[0])]["format"] = "blank";
            }
          }
          cat[static_cast<const string &>(item[0])]["item name"] = item[1];
        }
      } else {
        cat[static_cast<const string &>(item[0])]["format"] = "spare";
      }
      emplace(end(), cat[static_cast<const string &>(item[0])], item[0]);
    }
  }
  json &cat;
  json &uap;
  const string &name;
};

extern map<uint8_t, cat_spec> cats;  // cat objects
extern map<uint8_t, json> cats_json; // cat jsons
extern map<uint8_t, json> uap_json;  // uap jsons
extern vector<uint8_t> active_fields;

#define CAT_INIT                                                               \
  namespace nyancodec {                                                        \
  map<uint8_t, cat_spec> cats;                                                 \
  map<uint8_t, json> cats_json;                                                \
  map<uint8_t, json> uap_json;                                                 \
  vector<uint8_t> active_fields;                                               \
  }

void field_activate(uint8_t cat_id) { active_fields.push_back(cat_id); }

const cat_spec &cat_at(size_t id, size_t uap_id = 0) {
  if (cats.find(id) == cats.end()) {
    if (cats_json.find(id) == cats_json.end()) {
      ostringstream filename; // filename string
      filename << "cat/converted/cat" << setfill('0') << setw(3)
               << int(id); // add cat id to filename
      cout << STATUS << "Reading cat file" << filename.str() << ".json" << endl;
      ifstream ifs(filename.str() + ".json",
                   std::ifstream::in); // open file stream
      if (!ifs) {
        cout << ERR << "Error reading CAT file" << endl;
      } else {
        ifs >> cats_json[id]; // load cat json from stream
        ifs.close();          // done loading, close the stream
      }
      filename << "_uap" << int(uap_id); // load uap while were at it
      cout << STATUS << "Reading uap file: " << filename.str() << ".json"
           << endl;
      ifs.open(filename.str() + ".json", std::ifstream::in);
      if (!ifs) {
        cout << ERR << "Error reading UAP file" << endl;
      } else {
        ifs >> uap_json[id]; // load uap json from stream
        ifs.close();         // close the stream
      }
    }
    cout<<cats_json[id].dump(1)<<endl;
    cout<<uap_json[id].dump(1)<<endl;
    cats.emplace(id, pair<json &, json &>(cats_json[id], uap_json[id]));
  }
  map<uint8_t, json>::iterator it = cats_json.find(id);
  //  if (it == cats_json.end()) cout<<"FAILURE"<<endl;
  //  cout<<it.first<<it.second->("name")<<endl;
  return cats.at(id); // return reference to asked cat
}

// fspec
class fspec : public vector<size_t> {
public:
  void init(vector<oct>::iterator &it) {
    fspec_begin = it;
    size_t spec_index = 0;
    length = 0;
    do {
      if (*it & 0b10000000)
        push_back(spec_index);
      if (*it & 0b1000000)
        push_back(spec_index + 1);
      if (*it & 0b100000)
        push_back(spec_index + 2);
      if (*it & 0b10000)
        push_back(spec_index + 3);
      if (*it & 0b1000)
        push_back(spec_index + 4);
      if (*it & 0b100)
        push_back(spec_index + 5);
      if (*it & 0b10)
        push_back(spec_index + 6);
      spec_index += 7;
      length++;
    } while (*(it++) & 0b1);
    fspec_end = it;
  }
  void to_text_stream(auto &oss) {
    oss << MAGENTA << "FSPEC: " << RESET;
    hex_to_text_stream(fspec_begin, fspec_end, oss);
    oss << endl;
  }

  vector<oct>::iterator fspec_begin; // reference to msg raw data
  vector<oct>::iterator fspec_end;   // reference to msg raw data
  size_t length;
  // containing the start of this fspec
};
#ifdef BLE
// data field containing decoded data
class datafield {
public:
  datafield(const datafield_spec &dspec) : dspec(dspec) {}
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
  subfield(const subfield_spec &sspec) : sspec(sspec) {}
  void init(vector<oct>::iterator &it) {
    subfield_begin = it;
    reserve(sspec.size());
    for (const datafield_spec &ds : sspec) {
      push_back(ds);
    }
    it += sspec.byte_len;
    subfield_end = it;
  }
  void from_json(const json &j); // load from json
  void to_json(json &j);         // append to existing json
  json to_json();                // create new json
  ostream to_binary_stream();    // output to binary stream
  void to_string_stream(auto &oss) {
    hex_to_text_stream(subfield_begin, subfield_end, oss);
  }

  ostringstream to_string_stream(); // output to printable string stream
  void decode();                    // decode and store the data
  void encode(vector<oct>::iterator &msg_it); // fill up the msg_data block with
                                              // octets, corresponding to this
                                              // subfield contents
  vector<oct>::iterator subfield_begin;       // reference to msg raw data
  vector<oct>::iterator subfield_end;         // reference to msg raw data
  // containing the start of this subfield
  const subfield_spec &sspec; // reference to relevant subfield spec object
};

// item, containing subfields with decoded data
class item : public vector<subfield> {
public:
  item(const item_spec &ispec) : ispec(ispec) {}
  void init(vector<oct>::iterator &it) {
    item_begin = it;
    switch (ispec.item_type) {
    case itype::fix: // fixed length item
      reserve(ispec.size());
      for (const subfield_spec &ss : ispec) {
        push_back(ss);
        back().init(it);
      }
      break;
    case itype::ext: { // extended length item
      reserve(ispec.size());
      size_t i = 0;
      do {
        push_back(ispec[i]);
        back().init(it);
        i++;
      } while (*(it - 1) & 0b1);
      break;
    }
    case itype::var: // variable length item
      break;
    case itype::rep: // repetitive item
      break;
    case itype::com: // composite item
      fs.init(it);
      reserve(fs.size());
      for (auto &sub : fs) {
        push_back(ispec[sub]);
        back().init(it);
      }
      break;
    case itype::aux: // auxillary item
      reserve(1);
      push_back(ispec[0]);
      back().init(it);
      break;
    }
    item_end = it;
  }
  void from_json(const json &j); // parse from json
  void to_json(json &j);         // append to existing json
  json to_json();                // create new json
  ostream to_binary_stream();    // output to binary stream
  void to_text_stream(auto &oss) {
    oss << ispec.item["item_name"] << " ";
    hex_to_text_stream(item_begin, item_end, oss);
    oss << endl;
  }

  ostringstream to_text_stream(); // output to printable string stream
  void decode();                  // decode and store the data
  void encode(vector<oct>::iterator &msg_it); // fill up the msg_data block with
                                              // octets, corresponding to this
                                              // item contents
  vector<oct>::iterator item_begin;           // reference to msg raw data
  vector<oct>::iterator item_end;             // reference to msg raw data
  // containing the start of this item
  fspec fs;               // fspec(only for compound items)
  const item_spec &ispec; // reference to relevant item spec object
};

// block containing items with decoded data
class block : public vector<item> {
public:
  block(const cat_spec &cspec) : cspec(cspec) {}
  void init(vector<oct>::iterator &it) {
    fs.init(it);
    reserve(fs.size());
    for (auto i : fs) {
      push_back(cspec[i]);
      back().init(it);
    }
    block_end = it;
  }

  void parse(vector<oct>::iterator &it); // parse from msg data
  void from_json(const json &j);         // parse from json
  void to_json(json &j);                 // append to existing json
  json to_json();                        // create new json
  ostream to_binary_stream();            // output to binary stream
  void to_text_stream(auto &oss) {       // output to printable string stream
    fs.to_text_stream(oss);
    hex_to_text_stream(block_begin, block_end, oss);
    for (item &it : *this) {
      it.to_text_stream(oss);
    }
  }
  ostringstream to_text_stream(); // output to printable string stream
  void decode();                  // decode and store the data
  void encode(vector<oct>::iterator &msg_it); // fill up the msg_data block with
                                              // octets, corresponding to this
                                              // block contents
  vector<oct>::iterator block_begin;          // reference to msg raw data
  vector<oct>::iterator block_end;            // reference to msg raw data
  // containing the start of this block
  fspec fs;              // block fspec
  const cat_spec &cspec; // reference to relevant cat spec object
};
#endif
class datafield {
public:
  enum class status { blank, ready, err };
  datafield(const datafield_spec &dspec, vector<oct>::iterator &begin)
      : dspec(dspec), it_begin(begin) {
    curr = status::ready;
    switch (dspec.data_type) {
    case dtype::i:
      oct_to_long(it_begin, dspec.byte_loc, dspec.bit_loc, dspec.bit_len, i);
      break;
    case dtype::num:
      oct_to_long(it_begin, dspec.byte_loc, dspec.bit_loc, dspec.bit_len, i);
      f = i * dspec.lsb;
      break;
    case dtype::f:
      oct_to_long(it_begin, dspec.byte_loc, dspec.bit_loc, dspec.bit_len, i,
                  dspec.sign);
      f = i * dspec.lsb;
      break;
    case dtype::time:
      oct_to_long(it_begin, dspec.byte_loc, dspec.bit_loc, dspec.bit_len, i);
      f = i * dspec.lsb;
      break;
    case dtype::oct:
      it_begin += dspec.byte_loc;
      it_end = it_begin + (dspec.bit_len >> 3);
      break;
    case dtype::spare:
      break;
    case dtype::rep:
      oct_to_long(it_begin, dspec.byte_loc, dspec.bit_loc, dspec.bit_len, i);
      break;
    default:
      it_begin += dspec.byte_loc;
      it_end = it_begin + (dspec.bit_len >> 3);
      cout << WARNING << "Undefined handling data type "
           << dspec.datafield["type"] << endl;
      curr = status::err;
      break;
    }
  }
  datafield &operator=(const datafield &) { return *this; }
  const datafield_spec &dspec;
  void to_text_stream(auto &oss) {
    if (curr == status::ready) {
      switch (dspec.data_type) {
      case dtype::i:
        oss << static_cast<const string &>(dspec.datafield["name1"]) << " = "
            << i << ";" << endl;
        break;
      case dtype::num:
        oss << static_cast<const string &>(dspec.datafield["name1"]) << " = "
            << i << " "
            << static_cast<const string &>(dspec.datafield["options"][i][1])
            << ";" << endl;
        break;
      case dtype::f:
        oss << static_cast<const string &>(dspec.datafield["name1"]) << " = "
            << i << " (" << f << " "
            << static_cast<const string &>(dspec.datafield["unit"]) << ");"
            << endl;
        break;
      case dtype::time:
        oss << static_cast<const string &>(dspec.datafield["name1"]) << " = "
            << i << ";" << endl;
        break;
      case dtype::oct:
        oss << static_cast<const string &>(dspec.datafield["name1"])
            << " = 0x ";
        hex_to_text_stream(it_begin, it_end, oss);
        oss << ";" << endl;
        break;
      case dtype::spare:
        oss << flush;
        break;
      default:
        cout << WARNING << "Undefined handling data display "
             << static_cast<const string &>(dspec.datafield["type"]) << endl;
        oss << static_cast<const string &>(dspec.datafield["name1"])
            << " = 0x ";
        hex_to_text_stream(it_begin, it_end, oss);
        oss << ";" << endl;
        break;
      }
    } else {
      cout << WARNING << "Can not display decoded data "
           << static_cast<const string &>(dspec.datafield["type"]) << endl;
    }
  }

  long i;
  float f;
  string str;
  vector<oct>::iterator it_begin;
  vector<oct>::iterator it_end;
  status curr = status::blank;
};
class subfield : public vector<datafield> {
public:
  subfield(const subfield_spec &sspec, vector<oct>::iterator &it_begin,
           const vector<oct>::iterator &it_end)
      : sspec(sspec) {
    subfield_begin = it_begin;
    it_begin += sspec.byte_len;
    subfield_end = it_begin;
  }
  void decode() {
    switch (sspec.at(0).data_type) {
    case dtype::rep:{
      auto ds_it = sspec.begin();
      ds_it++;
      for (auto ds_it = sspec.begin() + 1; ds_it != sspec.end(); ds_it++) {
        emplace(end(), *ds_it, subfield_begin);
      }
      break;
    }
    default:
      for (const datafield_spec &ds : sspec) {
        emplace(end(), ds, subfield_begin);
      }
      break;
    }
  }

  void to_text_stream(auto &oss) {
    if (size() == 0)
      decode();
    for (datafield &d : *this) {
      d.to_text_stream(oss);
    }
  }

  subfield &operator=(const subfield &) { return *this; }
  const subfield_spec &sspec;
  vector<oct>::iterator subfield_begin; // reference to msg raw data
  vector<oct>::iterator subfield_end;   // reference to msg raw data
};
class item : public vector<subfield> {
public:
  item(const item_spec &ispec, vector<oct>::iterator &it_begin,
       const vector<oct>::iterator &it_end)
      : ispec(ispec) {
    item_begin = it_begin;
    switch (ispec.item_type) {
    case itype::fix: // fixed length item
      for (const subfield_spec &ss : ispec) {
        emplace(end(), ss, it_begin, it_end);
      }
      break;
    case itype::var: { // variable length item
      size_t i = 0;
      auto subfield_it = ispec.begin();
      do {
        if (subfield_it == ispec.end()) {
          cout << WARNING << static_cast<const string &>(ispec.name)
               << " End of variable-type item reached" << endl;
          break;
        }
        emplace(end(), *subfield_it, it_begin, it_end);
        subfield_it++;
      } while (*(it_begin - 1) & 0b1);
      break;
    }
    case itype::com: // composite item
      fs.init(it_begin);
      for (auto &sub : fs) {
        emplace(end(), ispec.at(sub), it_begin, it_end);
      }
      break;
    case itype::rep:
      oct_to_long(it_begin, 0, 0, ispec.at(0).at(0).bit_len, rep);
      it_begin += ispec.at(0).at(0).byte_len;
      for (auto i = 0; i < rep; i++) {
        emplace(end(), ispec.at(0), it_begin, it_end);
      }
      break;
    default:
      cout << WARNING << "Undefined handling item type " << ispec.item["format"]
           << endl;
      it_begin = it_end;
      break;
    }
    item_end = it_begin;
  }
  item &operator=(const item &) { return *this; }
  void to_text_stream(auto &oss) {
    oss << MAGENTABG << static_cast<const string &>(ispec.name) << RESET
        << ": ";
    hex_to_text_stream(item_begin, item_end, oss);
    oss << endl;
    for (subfield &sub : *this) {
      sub.to_text_stream(oss);
    }
  }

  const item_spec &ispec;
  vector<oct>::iterator item_begin; // reference to msg raw data
  vector<oct>::iterator item_end;   // reference to msg raw data
  fspec fs;                         // just in case the item is compound
  long rep;
};

class block : public vector<item> {
public:
  block(const cat_spec &cspec, vector<oct>::iterator &it_begin,
        const vector<oct>::iterator &it_end)
      : cspec(cspec) {
    block_begin = it_begin;
    fs.init(it_begin);
    for (auto item_index : fs) {
      if (it_begin == it_end) {
        cout << WARNING << "Block end reached prematurely" << endl;
        break;
      }
      this->emplace(end(), cspec.at(item_index), it_begin, it_end);
    }
    block_end = it_begin;
  }
  void to_text_stream(auto &oss) {
    fs.to_text_stream(oss);
    for (item &it : *this) {
      it.to_text_stream(oss);
    }
  }
  block &operator=(const block &rhs) {}

  const cat_spec &cspec;
  vector<oct>::iterator block_begin; // reference to msg raw data
  vector<oct>::iterator block_end;   // reference to msg raw data
  fspec fs;
};

// message, containing various blocks
// class msg : public vector<block> {
class msg : public vector<block> {
public:
  msg(auto &stream)
      : cat_id(int(stream.get())), msg_len((stream.get() << 8) + stream.get()),
        cspec(cat_at(cat_id)) {
    parse(stream);
  }
  void parse(auto &stream) { // parse from istream
    msg_data.resize(msg_len - 3);
    stream.read(reinterpret_cast<char *>(msg_data.data()), msg_data.size());
    auto data_iterator = msg_data.begin();
    while (data_iterator != msg_data.end()) {
      emplace(end(), cspec, data_iterator, msg_data.end());
    }
  }
  void to_text_stream(auto &oss) {
    oss << CYAN << "CAT: " << RESET << static_cast<uint>(cat_id) << endl;
    oss << CYAN << "LEN: " << RESET << static_cast<uint>(msg_len) << endl;
    hex_to_text_stream(msg_data.begin(), msg_data.end(), oss);
    oss << endl;
    for (block &b : *this) {
      b.to_text_stream(oss);
    }
  }
  uint8_t cat_id;
  uint16_t msg_len;
  const cat_spec &cspec;
  vector<oct> msg_data;
};
}

#endif // NYANCODEC_V2_HPP
