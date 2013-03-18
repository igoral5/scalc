/*
 * test_speed.cpp
 *
 *  Created on: 15.02.2012
 *      Author: igor
 */
#include "soapcalcProxy.h"
#include "calc.nsmap"
#include <ctime>
#include <cassert>
#include <cstdlib>


int main(int argc, char **argv)
{
    if (argc < 5) {
        std::cerr << "Use test_speed with param" << std::endl;
        return EXIT_FAILURE;
    }
    calcProxy calc;
    calc.soap_endpoint = argv[1];
    std::string in(argv[2]);
    std::string out;
    int count = atoi(argv[4]);
    for(int i = 0; i < count; ++i) {
        assert(!calc.evalute(in, false, out));
        assert(!calc.error);
        out = out.substr(0, out.size() - 1);
        assert(out == argv[3]);
        out.clear();
    }
    return EXIT_SUCCESS;
}




