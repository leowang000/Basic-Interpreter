/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"



Program::Program() = default;

Program::~Program() {
  clear();
}

void Program::clear() {
  for (auto iter = stmts.begin(); iter != stmts.end(); iter++) {
    delete iter->second;
  }
  stmts.clear();
  lines.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
  lines[lineNumber] = line;
}

void Program::removeSourceLine(int lineNumber) {
  if (lines.find(lineNumber) != lines.end()) {
    lines.erase(lineNumber);
    delete stmts[lineNumber];
    stmts.erase(lineNumber);
  }
}

std::string Program::getSourceLine(int lineNumber) {
  if (lines.find(lineNumber) != lines.end()) {
    return lines[lineNumber];
  }
  return "";
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
  if (stmts.find(lineNumber) != stmts.end()) {
    delete stmts[lineNumber];
  }
  stmts[lineNumber] = stmt;
}

Statement *Program::getParsedStatement(int lineNumber) {
  if (stmts.find(lineNumber) != stmts.end()) {
    return stmts[lineNumber];
  }
  return nullptr;
}

int Program::getFirstLineNumber() {
  if (lines.empty()) {
    return -1;
  }
  return lines.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
  auto iter = lines.upper_bound(lineNumber);
  if (iter == lines.end()) {
    return -1;
  }
  return iter->first;
}

void Program::stopRunning() {
  program_state = INPUT_STATE;
}

void Program::goTo(int target) {
  if (lines.find(target) == lines.end()) {
    error("LINE NUMBER ERROR");
  }
  now_line_number = target;
}

void Program::goToNextLine() {
  int nextLineNumber = getNextLineNumber(now_line_number);
  if (nextLineNumber == -1) {
    stopRunning();
    return;
  }
  goTo(nextLineNumber);
}

void Program::run(EvalState &state) {
  program_state = RUN_STATE;
  now_line_number = getFirstLineNumber();
  if (now_line_number == -1) {
    return;
  }
  while (program_state == RUN_STATE) {
    stmts[now_line_number]->execute(state, *this);
  }
}

void Program::list() {
  for (auto iter = lines.begin(); iter != lines.end(); iter++) {
    std::cout << iter->second << "\n";
  }
}

void Program::clear(EvalState &state) {
  clear();
  state.Clear();
}

void Program::quit() {
  for (auto iter = stmts.begin(); iter != stmts.end(); iter++) {
    delete iter->second;
  }
  exit(0);
}

void Program::help() {
  std::cout << "This is the help.\n";
}