#include "text.h"

int main(int argc, char const *argv[]){
    String s = "This is a single testsss!aaa";
    auto words = toGrams(s, 2);
    FOREACH<String>(words, [](int i, String &w){
        LOG(w);
    });

    
    return 0;
}
