/*
 * controlses.cpp
 *
 *  Created on: 14.02.2012
 *      Author: igor
 */

#include "controlses.h"
#include <syslog.h>

ControlBlock::ControlBlock() : ptr_(0), last_used_(0)
{
}

ControlBlock::ControlBlock(const ControlBlock& block)
{
    ptr_=block.ptr_;
    last_used_ = block.last_used_;
    block.ptr_ = 0;
}

void
ControlBlock::setCalcContext(CalcContext *calc)
{
    ptr_=calc;
}


ControlBlock::~ControlBlock() throw()
{
    delete ptr_;
}

time_t
ControlBlock::getLastUsed() const
{
    return last_used_;
}

PtrCalcContext::PtrCalcContext(ControlBlock *block) : block_(block)
{
}

PtrCalcContext::~PtrCalcContext() throw()
{
    block_->last_used_ = time(0);
}

CalcContext* PtrCalcContext::operator->()
{
    return block_->ptr_;
}




