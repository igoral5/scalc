// $Id: expression.h 48 2009-09-05 08:07:10Z tb $
/** \file expression.h Implements an example calculator class group. */

#ifndef CALCCONTEXT_H
#define CALCCONTEXT_H

#include <map>
#include <vector>
#include <stack>
#include <ostream>
#include <stdexcept>
#include <cmath>

/** Calculator context used to save the parsed expressions. This context is
 * passed along to the example::Driver class and fill during parsing via bison
 * actions. */
class CalcContext
{
public:
    /// type of the variable storage
    typedef std::map<std::string, double> variablemap_type;

    CalcContext();
     /// free the saved expression trees
    ~CalcContext();
    /// check if the given variable name exists in the storage
    bool existsVariable(const std::string &varname) const;
    /// return the given variable from the storage. throws an exception if it
    /// does not exist.
    double	getVariable(const std::string &varname) const;

    void setVariable(const std::string& varname, double value);

    double getResult() const;

    void setResult(double value);

    double getResultS(size_t index) const;

    void setResultS(double value);

    size_t getResultSize() const;

    void clearResultS();

    std::string getVarName();

    void setVarName(const std::string& name);
private:
    /// variable storage. maps variable string to doubles
    variablemap_type variables;

    std::stack<std::string> var_name;

    double result;

    std::vector<double> results;

};

#endif // CALCCONTEXT_H
