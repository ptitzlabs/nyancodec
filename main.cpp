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
    //ifstream ifs("sample_message.bin",std::ifstream::binary);
    ifstream ifs("Hackathon_202162.ast",std::ifstream::binary);
    ofstream ofs("results_complete.txt",std::ofstream::out | std::ofstream::app);
    // load the cat file
//    cat_load("cat/CAT062.json");
    // read the message
    uint counter = 0;
    while(counter < 10){
        msg m(ifs);
        m.to_text_stream(cout);
        counter++;

    }
    // print the message in the console
//    m.print();

    return 0;
}
