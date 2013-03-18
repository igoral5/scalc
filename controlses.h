/*
 * controlses.h
 *
 *  Created on: 14.02.2012
 *      Author: igor
 */

#ifndef CONTROLSES_H_
#define CONTROLSES_H_

#include <ctime>
#include "calccontext.h"
#include <boost/thread/mutex.hpp>


class ControlBlock
{
    friend class PtrCalcContext;
public:
    ControlBlock();
    ControlBlock(const ControlBlock& block);
    ~ControlBlock() throw();
    void setCalcContext(CalcContext *calc);
    time_t getLastUsed() const;
private:
    mutable CalcContext* ptr_;
    time_t last_used_;
};

class PtrCalcContext
{
public:
    PtrCalcContext(ControlBlock *block);
    ~PtrCalcContext() throw();
    CalcContext* operator->();
private:
    PtrCalcContext(const PtrCalcContext&);
    ControlBlock *block_;
};



#endif /* CONTROLSES_H_ */
