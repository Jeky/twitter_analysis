#include "utils.h"
#include "data.h"

int main(int argc, char const *argv[]){
    auto users = loadSpammers();
    LOG("Total users: ", users.size());
    
    return 0;
}