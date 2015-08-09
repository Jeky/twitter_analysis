#include "io.h"

void readFile(string &filename, function<bool (int, string&)> lineHandler){
    int i = 0;
    ifstream infile(filename);
    string line;

    if(!infile.good()){
        log() << "Cannot find file " << filename << endl;
        return;
    }

    log() << "Staring read file " << filename << endl;
    while(getline(infile, line)){
        if(i % LINE_COUNT == 0 && i != 0){
            log() << "Read " << i << " lines" << endl;
        }
        if(!lineHandler(i, line)){
            infile.close();
            return;
        }
        i++;
    }

    infile.close();
}
