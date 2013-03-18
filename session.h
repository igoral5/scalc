/*
 * session.h
 *
 *  Created on: 14.02.2012
 *      Author: igor
 */

#ifndef SESSION_H_
#define SESSION_H_

#include "calccontext.h"
#include "soapcalcService.h"
#include "controlses.h"
#include <vector>
#include <map>
#include <string>
#include <boost/thread/shared_mutex.hpp>


#define LEN_SESSIONID 13

class SessionManager
{
    friend SessionManager *getSessionManager();
    friend void GarbageCollector(time_t, time_t);
    typedef std::map<std::string, ControlBlock> context_t;
public:
    ControlBlock *getControlBlock(calcService *calcservice);
    boost::shared_mutex *getMutex() {
        return &used_;
    }
private:
    SessionManager();
    SessionManager(const SessionManager&);
    ~SessionManager() throw();
    ControlBlock *NewSession(calcService *calcservice);
    std::string idSession() const;
    context_t context_;
    boost::shared_mutex used_;
};

SessionManager *getSessionManager();

#endif /* SESSION_H_ */
