/*
	calcclient.cpp

	Example calculator service client in C++

	soapcpp2 -i calc.h
	cc -o calcclient calcclient.cpp stdsoap2.cpp soapC.cpp soapcalcProxy.cpp

--------------------------------------------------------------------------------
gSOAP XML Web services tools
Copyright (C) 2001-2008, Robert van Engelen, Genivia, Inc. All Rights Reserved.
This software is released under one of the following two licenses:
GPL or Genivia's license for commercial use.
--------------------------------------------------------------------------------
GPL license.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA

Author contact information:
engelen@genivia.com / engelen@acm.org
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
 */

#include "soapcalcProxy.h"
#include "calc.nsmap"

const char server[] = "http://localhost/cgi-bin/calcserver";

int main(int argc, char **argv)
{
    calcProxy calc;
    calc.soap_endpoint = server;
    std::string in;
    std::string out;
    while(std::cout << "Input: " && std::getline(std::cin, in) && !in.empty() )
    {
        calc.evalute(in, true, out);
        if (calc.error)
        {
            calc.soap_stream_fault(std::cerr);
        }
        else
        {
            std::cout << out;
        }
    }
    return 0;
}

