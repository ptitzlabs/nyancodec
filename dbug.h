#ifndef DBUG
#define DBUG
#include "include/console_color.hpp"
#define DBUG_MODE

namespace util{
enum class err{io,misc};
void print_err(const string & descr, err type = err::misc){
    using namespace std;
    switch(type){
    case err::io:
        cout<<RED<<"Error: "<<RESET<<"input/output error "<<descr<<endl;
        break;
    case err::misc:
        cout<<RED<<"Error: "<<RESET<<descr<<endl;
        break;
    }

}

#define WARNING BOLDYELLOW<<"Warning: "<<RESET
#define ERR BOLDRED<<"Error: "<<RESET
#define STATUS BOLDWHITE<<"Status: "<<RESET
}

#endif // DBUG

