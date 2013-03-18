%{
#include <string>
#include <sstream>
#include "scanner.h"
typedef example::Parser::token token;
typedef example::Parser::token_type token_type;
#define yyterminate() return token::END
%}
%option c++
%option batch
%option debug
%option yywrap nounput 
%option stack
%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}
%%
%{
    yylloc->step();
%}


[ \t\r]     { 
                yylloc->step();
            }
"="       return token::ASSIGN;
"%%"      return token::PEXPR;
"+"        return token::PLUS;
"-"        return token::MINUS;
"/"        return token::DIV;
"*"        return token::MUL;
"%"        return token::MOD;
"^"        return token::POW;
"("        return token::LP;
")"        return token::RP;
"pi()"     return token::PI;
"e()"      return token::E;
"cos("     return token::COS;
"sin("     return token::SIN;
"tg("      return token::TG;
"ctg("     return token::CTG;
"asin("    return token::ASIN;
"acos("    return token::ACOS;
"atg("     return token::ATG;
"actg("    return token::ACTG;
"gtor("    return token::GTOR;
"rtog("    return token::RTOG;
"gtogms("  return token::GTOGMS;
"gmstog("  return token::GMSTOG;
"lg("      return token::LG;
"ln("      return token::LN;
","        return token::COMMA;
"sqrt("    return token::SQRT;
"root("    return token::ROOT;
"pow("     return token::POW_F;
";"        return token::DOT_COMMA;
"\n"      {
              yylloc->lines(yyleng); 
              yylloc->step();
              return token::EOLN;
          }
[0-9]+    { 
              std::istringstream iss(yytext);
              iss >> yylval->value;
              return token::NUMBER;
          }
[0-9]*\.?[0-9]+((e|E)(\+|-)?[0-9]+)? { 
                                         std::istringstream iss(yytext);
                                         iss >> yylval->value;
                                         return token::NUMBER;
                                     }
[_a-zA-Z]+[_a-zA-Z0-9]*              { 
                                         calc->setVarName(yytext); 
                                         return token::NAME; 
                                     }
. {
    return static_cast<token_type>(*yytext);
}

%%
namespace example {

Scanner::Scanner(PtrCalcContext& calc_, std::istream* in,
		 std::ostream* out)
    : yyFlexLexer(in, out), calc(calc_)
{
}

Scanner::~Scanner()
{
}

void Scanner::set_debug(bool b)
{
    yy_flex_debug = b;
}

}
#ifdef yylex
#undef yylex
#endif

int yyFlexLexer::yylex()
{
    *yyout << "in yyFlexLexer::yylex() !" << std::endl;
    return 0;
}

int yyFlexLexer::yywrap()
{
    return 1;
}
