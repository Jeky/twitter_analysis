#include "utils.h"
#include "data.h"

int main(int argc, char const *argv[]){
    auto users = loadNonSpammers();
    LOG("Total users: ", users.size());
    
    return 0;
}
