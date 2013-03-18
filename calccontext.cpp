/*
 * calc.cpp
 *
 *  Created on: 07.02.2012
 *      Author: igor
 */
#include "calccontext.h"
#include "syslog.h"

CalcContext::CalcContext() : result(0.0)
{
    syslog(LOG_LOCAL0|LOG_DEBUG, "Create CalcContext");
}

CalcContext::~CalcContext()
{
    syslog(LOG_LOCAL0|LOG_DEBUG, "Destroy CalcContext");
}

bool CalcContext::existsVariable(const std::string& varname) const
{
    return variables.find(varname) != variables.end();
}

double  CalcContext::getVariable(const std::string& varname) const
{
    variablemap_type::const_iterator vi = variables.find(varname);
    if (vi == variables.end())
        throw(std::runtime_error("Unknown variable."));
    else
        return vi->second;
}

void CalcContext::setVariable(const std::string& varname, double value)
{
    variables[varname] = value;
}

double CalcContext::getResult() const
{
    return result;
}

void CalcContext::setResult(double value)
{
    result = value;
}

double CalcContext::getResultS(size_t index) const
{
    return results.at(index);
}

void CalcContext::setResultS(double value)
{
    results.push_back(value);
}

size_t CalcContext::getResultSize() const
{
    return results.size();
}

void CalcContext::clearResultS()
{
    results.clear();
}

std::string CalcContext::getVarName()
{
    if (var_name.empty())
        throw(std::runtime_error("Stack name variables empty"));
    std::string name(var_name.top());
    var_name.pop();
    return name;
}

void CalcContext::setVarName(const std::string& name)
{
    var_name.push(name);
}
