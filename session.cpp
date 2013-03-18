/*
 * session.cpp
 *
 *  Created on: 14.02.2012
 *      Author: igor
 */

#include "session.h"
#include "stdsoap2.h"
#include "sstream"
#include <ctime>
#include <cstdlib>
#include <syslog.h>

SessionManager *
getSessionManager()
{
    static SessionManager session_manager;
    return &session_manager;
}

SessionManager::SessionManager()
{
}
SessionManager::~SessionManager() throw()
{
}

ControlBlock *
SessionManager::getControlBlock(calcService *calcservice)
{
    soap_free_cookies(calcservice);
    soap_getenv_cookies(calcservice);
    char *s = soap_cookie_value(calcservice, "sessionid", NULL, NULL);
    if (s) {
        if (context_.find(s) != context_.end()) {
            syslog(LOG_LOCAL0|LOG_DEBUG, "Used old session %s", s);
            return &context_[s];
        } else {
            return NewSession(calcservice);
        }
    } else {
        return NewSession(calcservice);
    }
}

ControlBlock *
SessionManager::NewSession(calcService *calcservice)
{
    std::string sessionid = idSession();
    context_[sessionid].setCalcContext(new CalcContext);
    syslog(LOG_LOCAL0|LOG_DEBUG, "Create new session %s", sessionid.c_str());
    soap_set_cookie(calcservice, "sessionid", sessionid.c_str(), NULL, NULL);
    soap_set_cookie_session(calcservice, "sessionid", NULL, NULL);
    return &context_[sessionid];
}

std::string
SessionManager::idSession() const
{
    static char symbol[] ="_AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789";
    static unsigned int seed = time(0);
    char id[LEN_SESSIONID];
    id[LEN_SESSIONID -1] = '\0';
    do {
        for (size_t i = 0; i < LEN_SESSIONID - 1; ++i) {
            id[i] = symbol[rand_r(&seed) % (sizeof(symbol) - 1)];
        }
    } while (context_.find(id) != context_.end());
    return id;
}


