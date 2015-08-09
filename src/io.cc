#include "io.h"

void readFile(string &filename, bool log, 
    function<bool (int, string&)> lineHandler){
    int i = 0;
    ifstream infile(filename);
    string line;

    if(!infile.good()){
        if(log){
            LOG() << "Cannot find file " << filename << endl;
        }
        return;
    }

    if(log){
        LOG() << "Staring read file " << filename << endl;
    }
    while(getline(infile, line)){
        if(i % LINE_COUNT == 0 && i != 0){
            if(log){
                LOG() << "Read " << i << " lines" << endl;
            }
        }
        if(!lineHandler(i, line)){
            infile.close();
            return;
        }
        i++;
    }

    infile.close();
}

void readFile(string &filename, 
    function<bool (int, string&)> lineHandler){
    return readFile(filename, true, lineHandler);
}