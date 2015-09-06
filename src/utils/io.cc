#include "utils/io.h"

void readFile(const string &filename, bool log,
              function<bool(int, string &)> lineHandler) {
    int i = 0;
    ifstream infile(filename);
    string line;

    if (!infile.good()) {
        ERROR("Cannot find file ", filename);
        return;
    }

    if (log) {
        LOG("Staring read file ", filename);
    }
    while (getline(infile, line)) {
        if (i % LINE_COUNT == 0 && i != 0) {
            if (log) {
                LOG("Read ", i, " lines");
            }
        }
        if (!lineHandler(i, line)) {
            infile.close();
            return;
        }
        i++;
    }

    infile.close();
}

void readFile(const string &filename,
              function<bool(int, string &)> lineHandler) {
    return readFile(filename, true, lineHandler);
}

void writeFile(const string &filename, function<void(ofstream &)> writer) {
    LOG("Writing To ", filename);
    ofstream out(filename);
    writer(out);

    out.close();
    LOG("Finish.");
}