%{
#include <iostream>
#include <string>
#include <vector>
#include "calccontext.h"
%}
%require "2.3"
%debug
%start list
%defines
%skeleton "lalr1.cc"
%name-prefix="example"
%define "parser_class_name" "Parser"
%locations
%initial-action
{
    @$.begin.filename = @$.end.filename = &driver.streamname;
};
%parse-param { class Driver& driver }
%error-verbose
%union {
    double 	value;
}

%token          END          0  "end of file"
%token <value> 	NUMBER		    "number"
%token 			EOLN			"end of line" 
%token			PI 				"pi()"
%token			E 				"e()"
%token			PEXPR 			"previous expression"
%token			NAME 			"variable name"
%token			DOT_COMMA 		"semicolon"
%token			COMMA 			"comma"
%token			SQRT 			"square root"
%token			ROOT			"root of n-degree"
%token 			LP 				"left parenthesis"
%token			RP 				"right parenthesis"
%token			SPACE 			"space"
%token			SIN 			"sin()"
%token			COS 			"cos()"
%token			TG 				"tg()"
%token			CTG 			"ctg()"
%token			GTOR 			"degrees to radians"
%token			RTOG 			"radians to degrees"
%token			GTOGMS 			"degrees to degrees, minutes, seconds"
%token			GMSTOG 			"degrees, minutes and seconds to degrees"
%token			ASIN 			"arcsin()"
%token			ACOS 			"arccos()"
%token			ATG 			"arctg()"
%token			ACTG 			"arcctg()"
%token			LG 				"logarithm to base ten"
%token			LN				"natural logarithm"
%token			POW_F			"power"
%right ASSIGN POW
%left PLUS MINUS MUL DIV MOD
%type <value>	expr expr_assign expr_add expr_mul expr_pow unary_expr primary fun_call


%{
#include "driver.h"
#include "scanner.h"
#include "controlses.h"
#undef yylex
#define yylex driver.lexer->lex
%}
%%
list        : /* пусто */
            | list EOLN
            | list expr END
                {
                    driver.calc->setResultS($2);
                    driver.calc->setResult($2);
                    if (driver.trace_evalute)
                		driver.out_ << "Вычисленно выражение: " << $2 << std::endl;
                }
            | list expr EOLN
                { 
                	driver.calc->setResultS($2);
                	driver.calc->setResult($2);
                	if (driver.trace_evalute)
                		driver.out_ << "Вычисленно выражение: " << $2 << std::endl;
                }
            | list expr DOT_COMMA
                { 
                	driver.calc->setResultS($2);
                	driver.calc->setResult($2);
                	if (driver.trace_evalute)
                		driver.out_ << "Вычисленно выражение: " << $2 << std::endl;
                }
            ;
expr        : expr_assign
            ;        
expr_assign : expr_add
            | NAME ASSIGN expr_assign { 
                                          std::string name(driver.calc->getVarName());
                                          driver.calc->setVariable(name, $3);
                                          $$ = $3;
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Присвоено значение переменной " << name << " = " << $3 << std::endl;
                                      }   
            ;        
expr_add    : expr_mul
            | expr_add PLUS expr_mul  { 
            							  $$ = $1 + $3;
            							  if (driver.trace_evalute) 
            							      driver.out_ << "Сложение " << $1 << " + " << $3 << " = " << $$ << std::endl; 
            						  }
            | expr_add MINUS expr_mul { 
            							  $$ = $1 - $3;
            							  if (driver.trace_evalute) 
            				  			      driver.out_ << "Вычитание " << $1 << " - " << $3 << " = " << $$ << std::endl; 
            				  		  }
            ;
expr_mul    : expr_pow
            | expr_mul MUL expr_pow   { 
            							  $$ = $1 * $3;
            							  if (driver.trace_evalute) 
            							      driver.out_ << "Умножение " << $1 << " * " << $3 << " = " << $$ << std::endl; 
            						  }
            | expr_mul DIV expr_pow   { 
            							  $$ = $1 / $3;
            							  if (driver.trace_evalute) 
            							      driver.out_ << "Деление " << $1 << " / " << $3 << " = " << $$ << std::endl; 
            						  }
            | expr_mul MOD expr_pow   { 
            							  $$ = fmod($1, $3);
            							  if (driver.trace_evalute) 
            							      driver.out_ << "Деление по модулю " << $1 << " %% " << $3 << " = " << $$ << std::endl; 
            						  } 
            ;
expr_pow    : unary_expr
            | unary_expr POW expr_pow { 
            							  $$ = pow($1, $3);
            							  if (driver.trace_evalute) 
            							      driver.out_ << "Возведение в степень " << $1 << " ^ " << $3 << " = " << $$ << std::endl; 
            						  }
            ;
unary_expr  : primary
            | MINUS primary           { 
                                          $$ = -$2;
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Унарный минус " << $2 << " = " << $$ << std::endl; 
                                      }
            | PLUS primary            { 
            							  $$ = $2;
            							  if (driver.trace_evalute) 
            							      driver.out_ << "Унарный плюс " << $2 << " = " << $$ << std::endl; 
									  }
            ;
primary     : NUMBER                  { 
										  $$ = $1;
										  if (driver.trace_evalute) 
										      driver.out_ << "Число " << $$ << std::endl; 
									  }
            | NAME                    {
                                          std::string name(driver.calc->getVarName());
                                          $$ = driver.calc->getVariable(name);
                                          if (driver.trace_evalute)
                                              driver.out_ << "Использовано значение переменной " << name << " = " << $$ << std::endl; 
                   					  }
            | PEXPR                   {
            							  $$ = driver.calc->getResult();
            							  if (driver.trace_evalute) 
            							      driver.out_ << "Предыдущий результат " << $$ << std::endl; 
            						  }
            | LP expr RP              { 
            							  $$ = $2;
            							  if (driver.trace_evalute) 
            							      driver.out_ << "Результат действий в скобках " << $$ << std::endl;
									  }
            | fun_call
            ;
fun_call    : PI                      { 
                                          $$ = M_PI;
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов числа pi " << $$ << std::endl; 
                                      }
            | E                       { 
                                          $$ = M_E;
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов числа e " << $$ << std::endl;
                                      }
            | SIN expr RP             { 
                                          $$ = sin($2);
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов функции синус sin(" << $2 << ")=" << $$ << std::endl; 
                                      }
            | COS expr RP             { 
                                          $$ = cos($2);
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов функции косинус cos(" << $2 << ")=" << $$ << std::endl;
                                      }
            | TG expr RP              { 
                                          $$ = tan($2);
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов функции тангенс tg(" << $2 << ")=" << $$ << std::endl;
                                      }
            | CTG expr RP             { 
                                          $$ = 1/tan($2);
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов функции котангенс ctg(" << $2 << ")=" << $$ << std::endl; 
                                      }
            | GTOR expr RP            {
                                          $$ = $2 * M_PI / 180.0;
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов функции перевода градусов в радианы gtor(" << $2 << ")=" << $$ << std::endl; 
                                      }
            | RTOG expr RP            {
                                          $$ = $2 * 180.0 / M_PI;
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов функции перевода радиан в градусы rtog(" << $2 << ")=" << $$ << std::endl; 
                                      }
            | GTOGMS expr RP          { 
                                          double frac, integ, gms; 
                                          frac=modf($2, &gms); 
                                          frac *= 60.0; 
                                          frac = modf(frac, &integ);
                                          gms += integ / 100;
                                          frac *= 60;
                                          gms += frac / 10000;
                                          $$ = gms;
                                          if (driver.trace_evalute)
                                              driver.out_ << "Перевод градусов в градусы, минуты и секунды gmstog(" << $2 << ")=" << $$ << std::endl; 
                                      }
            | GMSTOG expr RP          { 
                                          double frac = $2 * 10000;
                                          int gms = nearbyint(frac);
                                          frac -= gms;
                                          int g = gms / 10000;
                                          gms -= g * 10000;
                                          int m = gms / 100;
                                          gms -= m * 100;
                                          frac += gms;
                                          frac /= 60.0;
                                          frac += m;
                                          frac /= 60.0;
                                          frac += g;
                                          $$ = frac;
                                          if (driver.trace_evalute)
                                              driver.out_ << "Перевод градусов, минут и секунд в градусы gmstog(" << $2 << ")=" << $$ << std::endl;  
                                      }
            | ASIN expr RP            { 
                                          $$ = asin($2);
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов функции арксинус asin(" << $2 << ")=" << $$ << std::endl; 
                                      }
            | ACOS expr RP            { 
                                          $$ = acos($2);
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов функции арккосинус acos(" << $2 << ")=" << $$ << std::endl;
                                      }
            | ATG expr RP             { 
                                          $$ = atan($2);
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов функции арктангенс atg(" << $2 << ")=" << $$ << std::endl; 
                                      }
            | ACTG expr RP            { 
                                          $$ = atan(1.0/$2);
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов функции арккотангенс actg(" << $2 << ")=" << $$ << std::endl;
                                      }
            | LN expr RP              { 
                                          $$ = log($2);
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов функции натуральный логарифм ln(" << $2 << ")=" << $$ << std::endl;
                                      }
            | LG expr RP              { 
                                          $$ = log10($2);
                                          if (driver.trace_evalute) 
                                              driver.out_ << "Вызов функции десятичный логарифм lg(" << $2 << ")=" << $$ << std::endl;
                                      }
            | SQRT expr RP            {
                                          $$ = sqrt($2);
                                          if (driver.trace_evalute)
                                              driver.out_ << "Извлечение квадратного корня sqrt(" << $2 << ")=" << $$ << std::endl;
                                      }
            | ROOT expr COMMA expr RP     {
                                              $$ = pow($2, 1.0/$4);
                                              if (driver.trace_evalute)
                                                   driver.out_ << "Извлечение корня " << $4 << " степени root(" << $2 << ", " << $4 << ")=" << $$ << std::endl;
                                          }
            | POW_F expr COMMA expr RP      {
            								  $$ = pow($2, $4);
            								  if (driver.trace_evalute)
            								       driver.out_ << "Возведение числа " << $2 << " в степень " << $4 << " pow(" << $2 << ", " << $4 << ")=" << $$ << std::endl;
            							  }
            ;
%%

void example::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}
