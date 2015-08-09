#include "utils.h"
#include "data.h"

int main(int argc, char const *argv[]){
    Map<int, String> map = {
        {1, String("aaa")},
        {3, String("bbb")},
        {5, String("cc")},
    };

    FOREACH<int, String>(map, [](int &k, String &v){
        LOG() << k << " = " << v << endl; 
    });


    return 0;
}