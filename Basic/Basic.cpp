/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string, Program &, EvalState &);

/* Main program */

int main() {
  EvalState state;
  Program program;
  //cout << "Stub implementation of BASIC" << endl;
  while (true) {
    try {
      std::string input;
      getline(std::cin, input);
      if (input.empty())
        return 0;
      processLine(input, program, state);
    } catch (ErrorException &ex) {
      if (ex.getMessage() == "DIVIDE BY ZERO" || ex.getMessage() == "VARIABLE NOT DEFINED" ||
          ex.getMessage() == "LINE NUMBER ERROR") {
        std::cout << ex.getMessage() << "\n";
      }
      else {
        std::cout << "SYNTAX ERROR" << "\n";
      }
    }
  }
  return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
  std::string token;
  bool execute_immediately;
  int line_number = 0;
  Statement *stmt = nullptr;
  Expression *exp = nullptr, *l_exp = nullptr, *r_exp = nullptr, *target = nullptr;
  TokenScanner scanner;
  scanner.ignoreWhitespace();
  scanner.scanNumbers();
  scanner.setInput(line);
  token = scanner.nextToken();
  execute_immediately = (scanner.getTokenType(token) != NUMBER);
  try {
    if (!execute_immediately) {
      line_number = stringToInteger(token);
      token = scanner.nextToken();
    }
    if (token.empty() && line_number > 0) {
      program.removeSourceLine(line_number);
    }
    else if (token == "RUN") {
      program.run(state);
    }
    else if (token == "LIST") {
      program.list();
    }
    else if (token == "CLEAR") {
      program.clear(state);
    }
    else if (token == "QUIT") {
      program.quit();
    }
    else if (token == "HELP") {
      program.help();
    }
    else if (token == "LET") {
      exp = readE(scanner);
      if (exp->getType() != COMPOUND || ((CompoundExp *) exp)->getOp() != "=") {
        error("SYNTAX ERROR");
      }
      stmt = new letStmt(exp);

    }
    else if (token == "PRINT") {
      exp = parseExp(scanner);
      stmt = new printStmt(exp);
    }
    else if (token == "INPUT") {
      exp = parseExp(scanner);
      if (exp->getType() != IDENTIFIER) {
        error("SYNTAX ERROR");
      }
      stmt = new inputStmt(((IdentifierExp *) exp)->getName());
      delete exp;
    }
    else {
      if (execute_immediately) {
        error("SYNTAX ERROR");
      }
      if (token == "REM") {
        stmt = new remStmt;
      }
      else if (token == "END") {
        if (scanner.hasMoreTokens()) {
          error("SYNTAX ERROR");
        }
        stmt = new endStmt;
      }
      else if (token == "GOTO") {
        exp = parseExp(scanner);
        if (exp->getType() != CONSTANT) {
          error("SYNTAX ERROR");
        }
        stmt = new gotoStmt(((ConstantExp *) exp)->getValue());
        delete exp;
      }
      else if (token == "IF") {
        l_exp = readE(scanner, 1);
        std::string op = scanner.nextToken();
        r_exp = readE(scanner, 1);
        scanner.nextToken();
        target = parseExp(scanner);
        if ((op != "=" && op != "<" && op != ">") || target->getType() != CONSTANT) {
          error("SYNTAX ERROR");
        }
        stmt = new ifStmt(l_exp, r_exp, op, ((ConstantExp *) target)->getValue());
        delete target;
      }
      else {
        error("SYNTAX ERROR");
      }
    }
    if (stmt != nullptr) {
      if (execute_immediately) {
        stmt->execute(state, program);
        delete stmt;
      }
      else {
        program.addSourceLine(line_number, line);
        program.setParsedStatement(line_number, stmt);
      }
    }
  } catch (ErrorException &ex) {
    if (stmt == nullptr) {
      delete exp;
      delete l_exp;
      delete r_exp;
      delete target;
    }
    else {
      delete stmt;
    }
    error(ex.getMessage());
  }
}