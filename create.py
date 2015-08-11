#!/usr/bin/env python

import os
import sys

INCLUDE = "include"
SOURCE = "src"


def createFiles(module, name):
    if not os.path.exists(module):
        os.mkdir(module)
        os.mkdir(module + '/' + INCLUDE)
        os.mkdir(module + '/' + SOURCE)

    f = open(module + '/' + INCLUDE + '/' + name + '.h', 'w')
    f.write('#ifndef _' + name.upper() + '_H_\n' +
            '#define _' + name.upper() + '_H_\n\n' +
            '#include "utils.h"\n\n\n' +
            '#endif\n')
    f.close()
    print module + '/' + INCLUDE + '/' + name + '.h', 'created!'

    f = open(module + '/' + SOURCE + '/' + name + '.cc', 'w')
    f.write('#include "' + name + '.h"\n\n')

    f.close()
    print module + '/' + SOURCE + '/' + name + '.cc', 'created!'


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print './create.py MODULE_NAME FILE_NAME'
    else:
        createFiles(sys.argv[1], sys.argv[2])
