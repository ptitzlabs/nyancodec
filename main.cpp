#include "nyancodec_v2.hpp"
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;


//namespace nyancodec{
//unordered_map<uint16_t,cat> cat_map; // category spec buffer
//}
CAT_INIT

int main() {
    using namespace nyancodec;
//    cout<<util::nyan_s;

    // load sample from binary file
    ifstream ifs("sample_message.bin",std::ifstream::binary);
    // load the cat file
//    cat_load("cat/CAT062.json");
    // read the message
    msg m(ifs);

  	std::ofstream ofs;
  	ofs.open ("test.txt", std::ofstream::out | std::ofstream::app);

  	m.to_text_stream_diff(ofs);
  	ofs.close();

  	m.to_text_stream_diff(cout);
    // print the message in the console
//    m.print();

    return 0;
}
