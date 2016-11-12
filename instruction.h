/* FILE INFORMATION
File:     instruction.h
Authors:  Matthew Cole <mcole8@binghamton.edu>
          Brian Gracin <bgracin1@binghamton.edu>
Description: Header for instruction.cpp
*/

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

struct instruction_t {
  int pc; // the Program Counter value of this instruction
  std::string opcode; //the string (e.g. ADD) representing this instruction
  std::vector<std::string> operands; //a vector of strings representing operands
}

int littos(string literal); //Convert a literal string to a 4-byte integral

#endif
