/*
	calcserver.cpp

	Example calculator service in C++

	soapcpp2 -i calc.h
	cc -o calcserver++ calcserver.cpp stdsoap2.cpp soapC.cpp soapcalcService.cpp

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

#include "soapcalcService.h"
#include "calc.nsmap"
#include "calccontext.h"
#include "driver.h"
#include "session.h"
#include "controlses.h"
#include <sstream>
#include <boost/thread/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <time.h>
#include <syslog.h>

void
GarbageCollector(time_t sleep, time_t expired)
{
    struct timespec req, rem;
    req.tv_sec = sleep;
    req.tv_nsec = 0;
    SessionManager *session = getSessionManager();

    for(;;) {
        rem.tv_sec = 0;
        rem.tv_nsec = 0;
        nanosleep(&req, &rem);
        boost::unique_lock<boost::shared_mutex> lock(*session->getMutex());
        syslog(LOG_LOCAL0|LOG_DEBUG, "Start GarbageCollector");
        SessionManager::context_t::iterator i = session->context_.begin();
        SessionManager::context_t::iterator i_end = session->context_.end();
        SessionManager::context_t::iterator j = i;
        for(; i != i_end; j = i) {
            ++i;
            syslog(LOG_LOCAL0|LOG_DEBUG, "Session %s last used %ld current time %ld", j->first.c_str(), j->second.getLastUsed(), time(0));
            if (j->second.getLastUsed() + expired < time(0)) {
                syslog(LOG_LOCAL0|LOG_DEBUG, "Delete session %s", j->first.c_str());
                session->context_.erase(j);

            }
        }
    }
}

int main(int argc, char **argv)
{
    calcService calcserve;
    calcserve.cookie_domain = "localhost";
    openlog("calcserver", LOG_PID|LOG_CONS|LOG_NDELAY|LOG_NOWAIT, LOG_LOCAL0);
    setlogmask(LOG_UPTO(LOG_DEBUG));
    syslog(LOG_LOCAL0|LOG_INFO, "Start calcserver");
    boost::thread col(GarbageCollector, 60, 120);
    col.detach();
    if (argc < 2) {
        calcserve.serve();	/* serve as FCGI application */
    }
    else
    {
        int port = atoi(argv[1]);
        if (!port)
        {
            fprintf(stderr, "Usage: calcserver++ <port>\n");
            exit(0);
        }
        /* run iterative server on port until fatal error */
        if (calcserve.run(port))
        {
            calcserve.soap_stream_fault(std::cerr);
            exit(-1);
        }
    }
    syslog(LOG_LOCAL0|LOG_INFO, "Down calcserver");
    return 0;
} 


int calcService::evalute(std::string in, bool trace_evalute, std::string& out)
{
    syslog(LOG_LOCAL0|LOG_DEBUG, "Start evalute with expr %s", in.c_str());
    std::ostringstream oss;
    SessionManager *session = getSessionManager();
    boost::shared_lock<boost::shared_mutex> lock(*session->getMutex());
    PtrCalcContext calc(session->getControlBlock(this));
    calc->clearResultS();
    example::Driver driver(calc, oss);
    driver.trace_evalute = trace_evalute;
    try {
        bool result = driver.parse_string(in, "input");
        if (result) {
            for (size_t i = 0; i < calc->getResultSize(); ++i) {
                oss << "Evalute" << "[" << i << "] : " << calc->getResultS(i) << std::endl;
            }
        }
    }
    catch (std::runtime_error& e) {
        oss << e.what() << std::endl;
    }
    out = oss.str();
    return SOAP_OK;
}
