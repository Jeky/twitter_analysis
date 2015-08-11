#include "ml.h"

int main(int argc, char const *argv[]){
    Instance ins;

    ins["a"] = 1.0;
    ins["b"] = 2.0;

    for(auto &kv : ins){
        LOG(kv.first, " = ", kv.second);
    }
    
    return 0;
}
