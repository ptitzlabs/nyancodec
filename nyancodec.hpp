#ifndef NYANCODEC_H
#define NYANCODEC_H
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

typedef unsigned char oct;

class cat;
// unordered map that stores the cat specifications
extern unordered_map<uint16_t, cat> cat_map;

// item data types
const array<string, 12> dtype_s = {
    "enum",           // enumerator
    "int",            // integer
    "unsigned int",   // unsigned integer
    "float",          // float
    "unsigned float", // unsigned float
    "variable float", // variable floating point
    "time",           // time
    "string",         // string
    "raw",            // raw bytes
    "spare",          // spare bits
    "fx",             // fx flag
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
  raw,    // raw bytes
  spare,  // spare bits
  fx,     // fx flag
  rep,    // repeat flag
};

// field types
const array<string, 5> ftype_s = {
    "fixed",     // fixed length field
    "extended",  // extended length field
    "repeating", // repetitive field
    "composite", // composite field
    "aux"        // auxillary field
};
enum class ftype {
  fix, // fixed length field
  ext, // extended length field
  rep, // repetitive field
  com, // composite field
  aux  // auxillary field
};

// specifies a single variable
class item_spec {
public:
  // constructors
  item_spec() {}
  item_spec(dtype type, uint8_t bit_len, string name)
      : data_type(type), bit_len(bit_len), name(name),
        sign(type == dtype::f || type == dtype::i) {}
  item_spec(dtype type, uint8_t bit_len, string name, string unit, float factor,
            float offset = 0.0)
      : data_type(type), bit_len(bit_len), name(name), unit(unit), lsb(factor),
        offset(offset), sign(type == dtype::f || type == dtype::i) {}
  item_spec(dtype type, uint8_t bit_len, string name, vector<string> enum_val)
      : data_type(type), bit_len(bit_len), name(name), enum_val(enum_val),
        sign(type == dtype::f || type == dtype::i) {
    if (enum_val.size() < static_cast<uint8_t>(2 << (bit_len - 1))) {
      this->enum_val.resize(2 << (bit_len - 1));
    }
  }
  item_spec(dtype type, uint8_t bit_len, string name, vector<string> enum_unit,
            vector<double> enum_factor)
      : data_type(type), bit_len(bit_len), name(name), enum_val(enum_unit),
        enum_factor(enum_factor), sign(type == dtype::f || type == dtype::i) {}

  item_spec(const item_spec &src)
      : data_type(src.data_type), bit_len(src.bit_len), name(src.name),
        unit(src.unit), lsb(src.lsb), offset(src.offset),
        enum_val(src.enum_val), enum_factor(src.enum_factor),
        oct_loc(src.oct_loc), bit_offs(src.bit_offs), item_id(src.item_id),
        sign(src.sign) {}
  item_spec &operator=(const item_spec &src) {
    item_id = src.item_id;
    oct_loc = src.oct_loc;
    data_type = src.data_type;
    bit_len = src.bit_len;
    bit_offs = src.bit_offs;
    if (data_type != dtype::fx && data_type != dtype::spare) {
      name = src.name;

      if (data_type == dtype::f || data_type == dtype::uf) {
        lsb = src.lsb;
        offset = src.offset;
        unit = src.unit;
      }

      if (data_type == dtype::num || data_type == dtype::vf) {
        enum_val = src.enum_val;
        if (data_type == dtype::vf) {
          enum_factor = src.enum_factor;
        }
      }
      sign = src.sign;
    }
    return *this;
  }
  // store item spec to json object
  json to_json() const {
    json j;
    j["item id"] = item_id;
    j["type"] = dtype_s[static_cast<int>(data_type)];
    j["bit begin"] = (oct_loc << 3) + bit_offs;
    j["bit end"] = static_cast<uint16_t>(j["bit begin"]) + bit_len;
    if (data_type != dtype::fx && data_type != dtype::spare) {
      j["name"] = name;
      if (data_type == dtype::f || data_type == dtype::uf) {
        j["LSB"] = lsb;
        j["offset"] = offset;
        j["unit"] = unit;
      }
      if (data_type == dtype::num || data_type == dtype::vf) {
        j["enum vals"] = enum_val;
      }
      if (data_type == dtype::vf) {
        j["var LSB"] = enum_factor;
      }
    }
    return j;
  }
  // load item spec from json object
  void load_json(const json &j) {
    uint count = 0;
    while (true) {
      if (dtype_s[count] == j["type"]) {
        data_type = static_cast<dtype>(count);
        break;
      }
      count++;
    }
    item_id = j["item id"];

    bit_len = static_cast<uint16_t>(j["bit end"]) -
              static_cast<uint16_t>(j["bit begin"]);
    oct_loc = static_cast<uint16_t>(j["bit begin"]) >> 3;
    bit_offs = static_cast<uint16_t>(j["bit begin"]) % 8;

    if (data_type != dtype::fx && data_type != dtype::spare) {
      name = j["name"];
      if (data_type == dtype::f || data_type == dtype::uf) {
        unit = j["unit"];
        lsb = j["LSB"];
        offset = j["offset"];
      }
      if (data_type == dtype::num || data_type == dtype::vf) {
        for (auto &a : j["enum vals"])
          enum_val.push_back(a);
      }
      if (data_type == dtype::vf) {
        for (auto &a : j["var LSB"])
          enum_factor.push_back(a);
      }
      sign = (data_type == dtype::f || data_type == dtype::i) ? true : false;
    }
  }

  dtype data_type;            // item type
  uint8_t bit_len;            // length in bits
  string name;                // item name
  string unit;                // floating point value unit
  double lsb;                 // floating point LSB
  double offset;              // floating point offset
  vector<string> enum_val;    // enumerator string values
  vector<double> enum_factor; // variable scaling float
  uint8_t oct_loc;            // byte location
  uint8_t bit_offs;           // bit offset
  uint8_t item_id;            // item id
  bool sign;                  // signed indicator (unsigned/signed)
};

// specifies a subfield, with several variables
class subfield_spec : public vector<item_spec> {
public:
  // constructors
  subfield_spec() {}
  subfield_spec(const subfield_spec &src)
      : vector<item_spec>(src), subfield_id(src.subfield_id),
        bit_len(src.bit_len), oct_len(src.oct_len) {}

  // assign ids, calculate octet and bit lengths of the subfield
  void assign_ids() {
    bit_len = 0;
    oct_len = 0;
    for (uint i : indices(*this)) {
      (*this)[i].item_id = i;
      (*this)[i].oct_loc = oct_len;
      (*this)[i].bit_offs = bit_len - (oct_len << 3);
      bit_len += (*this)[i].bit_len;
      oct_len = bit_len >> 3;
    }
  }
  // store subfield spec to json
  json to_json() const {
    json j;
    j["subfield id"] = subfield_id;
    j["length"] = this->size();
    for (const auto &it : *this) {
      j["items"].push_back(it.to_json());
    }
    return j;
  }
  // load subfield spec from json
  void load_json(const json &j) {
    subfield_id = j["subfield id"];
    bit_len = 0;
    oct_len = 0;
    size_t item_id;
    resize(j["length"]);
    if (j.find("items") != j.end()) {
      for (auto &it : j["items"]) {
        item_id = it["item id"];
        (*this)[item_id].load_json(it);
        bit_len += (*this)[item_id].bit_len;
      }
      oct_len = bit_len >> 3;
    }
  }

  uint8_t subfield_id;
  uint16_t bit_len;
  uint8_t oct_len;
};

// field specification, contains several subfields
class field_spec : public vector<subfield_spec> {
public:
  field_spec() {}
  field_spec(int8_t subfield_len, ftype type, string item_index = "",
             string info = "", string descr = "")
      : vector<subfield_spec>(subfield_len, subfield_spec()), field_type(type),
        index(item_index), info(info), descr(descr) {}
  field_spec(const field_spec &src)
      : vector<subfield_spec>(src), field_type(src.field_type), info(src.info),
        descr(src.descr) {}
  // assign ids, calculate octet length of the field
  void assign_ids() {
    oct_len = 0;
    for (uint i : indices(*this)) {
      (*this)[i].subfield_id = i;
      (*this)[i].assign_ids();
      oct_len += (*this)[i].oct_len;
    }
  }

  // put field spec into json
  json to_json() const {
    json j;
    j["field id"] = field_id;
    j["length"] = this->size();
    j["field type"] = ftype_s[static_cast<size_t>(field_type)];
    j["field index"] = index;
    j["info"] = info;
    j["description"] = descr;
    for (const auto &s : *this) {
      j["subfields"].push_back(s.to_json());
    }
    return j;
  }

  // load field spec from json
  void load_json(const json &j) {
    uint count = 0;
    while (true) {
      if (ftype_s[count] == j["field type"]) {
        field_type = static_cast<ftype>(count);
        break;
      }
      count++;
    }
    field_id = j["field id"];
    index = j["field index"];
    info = j["info"];
    descr = j["description"];

    size_t subfield_id;
    oct_len = 0;
    resize(j["length"]);
    if (j.find("subfields") != j.end()) {
      for (auto &sub : j["subfields"]) {
        subfield_id = sub["subfield id"];
        (*this)[subfield_id].load_json(sub);
        oct_len += (*this)[subfield_id].oct_len;
      }
    }
  }

  uint8_t field_id; // field id
  ftype field_type; // type (fixed/variable length)
  // UAP
  string index;    // field index
  string info;     // short name of the field
  string descr;    // long description of the field
  uint8_t oct_len; // length in octets
};
class cat : public vector<field_spec> {
public:
  cat() {}
  cat(const cat &src)
      : vector<field_spec>(src), name(src.name), definition(src.definition),
        description(src.description), cat_id(src.cat_id) {}

  void assign_ids() {
    for (auto i : indices(*this)) {
      (*this)[i].field_id = i;
      (*this)[i].assign_ids();
    }
  }
  void set_item_spec(uint8_t spec_index, uint8_t subfield_len, ftype type,
                     string index = "", string info = "", string descr = "") {
    (*this)[spec_index] = field_spec(subfield_len, type, index, info, descr);
  }
  string name;        // category name
  string definition;  // category definition
  string description; // category description
  uint8_t cat_id;     // CAT ID

  json to_json() { // put the cat to json
    json j;
    j["CAT id"] = cat_id;
    j["name"] = name;
    j["definition"] = definition;
    j["description"] = description;
    j["length"] = this->size();
    for (auto &f : *this) {
      if (f.field_type != ftype::aux)
        j["fields"].push_back(f.to_json());
    }
    return j;
  }

  void load_json(const json &j) { // load a cat from json
    cat_id = j["CAT id"];
    name = j["name"];
    definition = j["definition"];
    description = j["description"];
    resize(j["length"]);
    size_t field_id;
    for (auto &fld : j["fields"]) {
      field_id = fld["field id"];
      (*this)[field_id].load_json(fld);
    }
  }
};

class item {
public:
  item(const item_spec &spec, const vector<oct> *hex) : spec(spec), hex(hex) {
    switch (spec.data_type) {
    case dtype::raw:
      break;
    case dtype::string:
      break;
    default:
      grab_bits(
          hex->begin(), spec.oct_loc, spec.bit_offs, spec.bit_len, val,
          spec.sign); // grabs the bits, depending on item location and length
    }
  }
  item(const item &src) : spec(src.spec), hex(src.hex), val(src.val) {}

  // puts the item to string
  string to_string() {
    ostringstream oss;
    switch (spec.data_type) {
    case dtype::num: // enumerator type
      oss << spec.name << ": ";
      oss << bin_to_str(val, spec.bit_len) << " " << spec.enum_val[val];
      break;
    case dtype::ui: // unsigned integer type
      oss << spec.name << " = ";
      oss << dec << val;
      break;
    case dtype::uf: // unsigned float type
      oss << spec.name << " = ";
      oss << dec << val << " (" << val *spec.lsb + spec.offset << " "
          << spec.unit << ") ";
      break;
    case dtype::i: // signed integer type
      oss << spec.name << " = ";
      oss << dec << val;
      break;
    case dtype::f: // float
      oss << spec.name << " = ";
      oss << dec << val << " (" << val *spec.lsb + spec.offset << " "
          << spec.unit << ") ";
      break;
    case dtype::time: // time
      oss << spec.name << " = ";
      oss << dec << "0x" << hex_to_str(val) << " (" << val << "; "
          << int_to_time(val, spec.lsb) << " " << spec.unit << ")";
      break;
    case dtype::raw: // raw bytes
      oss << spec.name << ": ";
      for (auto i = 0; i < (spec.bit_len >> 3); i++) {
        oss << hex_to_str((*hex)[i], 1) << " ";
      }
      break;
    case dtype::string: { // string
      oss << spec.name << ": ";
      char str[spec.bit_len / 6];
      grab_string(hex->begin(), spec.bit_len, str, 6);
      oss << string(str, 6);
    } break;
    case dtype::vf:
      break;
    case dtype::spare:
      break;
    case dtype::rep:
      break;
    case dtype::fx:
      break;
    }
    return oss.str();
  }
  const item_spec &spec;
  const vector<oct> *hex;
  signed long val;
};

class subfield : public vector<item> {
public:
  subfield(const subfield_spec &spec)
      : sf_spec(spec), hex(vector<oct>(spec.oct_len)) {}
  subfield(const subfield &src)
      : vector<item>(src), sf_spec(src.sf_spec), hex(src.hex) {
    for (auto &it : *this) {
      it.hex = &hex;
    }
  }
  void fill(auto &stream) {
    hex.reserve(sf_spec.oct_len); // reserve enough space
    // grab a bunch of octets at once
    stream.read(reinterpret_cast<char *>(hex.data()), sf_spec.oct_len);
    (*this).reserve(sf_spec.size());   // reserve enough space for the items
    for (auto &s : sf_spec) {          // iterate for the whole subfield specs
      if (s.data_type != dtype::spare) // skip the spare items
        (*this).push_back(item(s, &hex)); // store items
    }
  }
  // reads an extended field from stream
  void fill_extended(auto &stream) {
    (*this).reserve(sf_spec.size()); // reserve enough space
    auto ds_it = sf_spec.begin();    // subfield spec iterator
    do {
      hex.push_back(stream.get());              // grab the octets one by one
      while ((*ds_it).data_type != dtype::fx) { // read the messages
        (*this).push_back(item(*ds_it, &hex));  // store items
        ds_it++;                                // increase subfield iterator
      }
      ds_it++;                  // skip the fx flag
    } while (hex.back() & 0b1); // iterate while fx is set
  }

  const subfield_spec &sf_spec;
  vector<oct> hex;
};

class field : public vector<subfield> {
public:
  const field_spec &f_spec;
  field(const field_spec &spec) : f_spec(spec) {}
  field(const field &src) : vector<subfield>(src), f_spec(src.f_spec) {}
  void fill(auto &stream) {
    switch (f_spec.field_type) {            // check what kind of a field it is
    case ftype::fix:                        // fixed length field
      this->push_back(subfield(f_spec[0])); // add a single subfield
      this->back().fill(stream);            // read the subfield from stream
      break;
    case ftype::com: { // compound field
      // first read which subfields are selected
      do {
        active_subfield.push_back(stream.get());
      } while (active_subfield.back() & bit_mask[7]);

      auto spec_iter = f_spec.begin(); // then iterate for all active ones
      for (auto fs : active_subfield) {
        for (auto i = 0; i < 7; i++) {
          if (fs & bit_mask[i]) {
            this->push_back(subfield(*spec_iter));
            this->back().fill(stream); // read the subfield from stream
          }
          spec_iter++;
        }
        spec_iter++;
      }
    } break;
    case ftype::ext:                        // extended length field
      this->push_back(subfield(f_spec[0])); // only one subfield
      this->back().fill_extended(stream);   // read from stream
      break;
    case ftype::rep: // repetitive field is not defined yet
      break;
    case ftype::aux: // auxillary fields are skipped
      break;
    }
  }
  string to_string() {
    ostringstream oss;
    oss << " " << YELLOW << f_spec.index << WHITE << ": 0x ";
    if (!active_subfield.empty()) {
      for (auto &f : active_subfield) {
        oss << hex_to_str(f, 1) << " ";
      }
    }
    for (auto &it : *this) {
      for (auto &h : it.hex) {
        oss << hex_to_str(h, 1) << " ";
      }
    }
    oss << endl;
    oss << " " << f_spec.info << ": ";
    switch (f_spec.field_type) {
    case ftype::fix:
      if ((*this)[0].size() <= 2) {
        for (auto &f : (*this)[0])
          oss << f.to_string() << " ";
      } else {
        for (auto &f : (*this)[0]) {
          oss << endl;
          oss << "  " << f.to_string();
        }
      }
      oss << endl;
      break;
    case ftype::com:
      for (auto &it : (*this)) {
        for (auto &f : it) {
          oss << endl;
          oss << "  " << f.to_string();
        }
      }
      oss << endl;
      break;
    case ftype::ext:
      if ((*this)[0].size() <= 2) {
        for (auto &f : (*this)[0])
          oss << f.to_string() << " ";
      } else {
        for (auto &f : (*this)[0]) {
          oss << endl;
          oss << "  " << f.to_string();
        }
      }
      oss << endl;
      break;
    case ftype::rep:
      break;
    case ftype::aux:
      break;
    }
    return oss.str();
  }
  vector<oct> active_subfield;
};
// block containing several fields
class block : public vector<field> {
public:
  block(const cat &spec) : c_spec(spec) {}
  block(const block &src) : vector<field>(src), c_spec(src.c_spec) {}
  const cat &c_spec;
  vector<oct> fspec;
  void fill(auto &stream) {
    // read the fspec
    do {
      fspec.push_back(stream.get());
    } while (fspec.back() & 0b1);

    // reserve enough space to store all the fields
    this->reserve(fspec.size() << 3);

    uint8_t item_id = 0; // iterate for all the items activated in fspec
    for (auto &f : fspec) {
      for (auto i = 0; i < 7; i++) {
        if (f & bit_mask[i]) {
          this->push_back(field(c_spec[item_id]));
          this->back().fill(stream); // pass on the stream to the item
        }
        item_id++;
      }
    }
  }
};
// message buffer, a message consists of multiple blocks
class msg : public vector<block> {
public:
  uint8_t cat_id;
  uint16_t n;
  const cat &spec;
  msg(const cat &spec) : spec(spec) {}
  msg(auto &stream) : cat_id(stream.get()), spec(cat_map[cat_id]) {
    n = (stream.get() << 8) + stream.get(); // grab the CAT number
    push_back(block(spec));      // add a block to the current message
    (*this).back().fill(stream); // fill from stream
  }
  // print on screen
  void print() {
    cout << "ASTERIX var block: cat=" << int(cat_id) << "; len=" << int(n)
         << endl << endl;
    for (block &b : *this) {
      cout << "FSPEC: 0x ";
      cout << internal << setfill('0');
      for (oct &fs : b.fspec) {
        cout << hex << setw(2) << int(fs) << " ";
      }
      cout << endl;
      cout << "Data Record:" << endl;
      for (field &it_set : b) {
        cout << it_set.to_string();
      }
    }
  }
};

// loads a cat from a file
inline void cat_load(const string &filename) {
  ifstream ifs(filename, std::ifstream::in); // grab a file stream
  json js;                                   // temp json
  ifs >> js;                                 // read to json
  cat_map[js["CAT id"]].load_json(js);       // load into cat
}
}
#endif
