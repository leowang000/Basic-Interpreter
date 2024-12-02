/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"


/* Implementation of the Statement class */

Statement::Statement() = default;

Statement::~Statement() = default;

remStmt::remStmt() = default;

remStmt::~remStmt() = default;

void remStmt::execute(EvalState &state, Program &program) {
  program.goToNextLine();
}

letStmt::letStmt(Expression *other) : exp(other) {}

letStmt::~letStmt() {
  delete exp;
}

void letStmt::execute(EvalState &state, Program &program) {
  exp->eval(state);
  program.goToNextLine();
}

printStmt::printStmt(Expression *other) : exp(other) {}

printStmt::~printStmt() {
  delete exp;
}

void printStmt::execute(EvalState &state, Program &program) {
  std::cout << exp->eval(state) << "\n";
  program.goToNextLine();
}

inputStmt::inputStmt(std::string other) : name(other) {}

inputStmt::~inputStmt() = default;

void inputStmt::execute(EvalState &state, Program &program) {
  std::string value;
  while (true) {
    try {
      std::cout << " ? ";
      getline(std::cin, value);
      state.setValue(name, stringToInteger(value));
      break;
    } catch (ErrorException &ex) {
      std::cout << "INVALID NUMBER\n";
    }
  }
  program.goToNextLine();
}

endStmt::endStmt() = default;

endStmt::~endStmt() = default;

void endStmt::execute(EvalState &state, Program &program) {
  program.stopRunning();
}

gotoStmt::gotoStmt(int target) : n(target) {}

gotoStmt::~gotoStmt() = default;

void gotoStmt::execute(EvalState &state, Program &program) {
  program.goTo(n);
}

ifStmt::ifStmt(Expression *l_exp, Expression *r_exp, std::string op, int target)
        : lhs(l_exp), rhs(r_exp), cmp(op), n(target) {}

ifStmt::~ifStmt() {
  delete lhs;
  delete rhs;
}

void ifStmt::execute(EvalState &state, Program &program) {
  int left = lhs->eval(state), right = rhs->eval(state);
  bool jump = (cmp == "=" && left == right) || (cmp == "<" && left < right) || (cmp == ">" && left > right);
  if (jump) {
    program.goTo(n);
    return;
  }
  program.goToNextLine();
}