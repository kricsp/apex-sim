/* FILE INFORMATION
File:     cpu.h
Authors:  Matthew Cole <mcole8@binghamton.edu>
          Brian Gracin <bgracin1@binghamton.edu>
Description: Header file for stage.cpp
*/

#include <string>
#include <vector>

#ifndef STAGE_H
#define STAGE_H

//Represents the instruction occupying an APEX pipeline stage, its operands, any values accumulated by the register file/forwarding/computation and any validity bools accumulated.
class Stage {
private:
    std::string name;

public:
  //the stage is occupied by an inflight instruction
  //(it will always have an instruction loaded, but that instruction may have
  // advanced to a follow-on stage; it should not be "simulated" again)
  bool isEmpty;
  //the stage has completed its work for the inflight instruction
  //(it is ready to be forwarded and/or advanced)
  bool isReady;


  //Instruction contents
  int pc; //program counter value that issued this
  std::string opcode; //(e.g. ADD) representing this instruction
  std::vector<std::string> operands;
  std::vector<int> values;
  std::vector<bool> valids;

  //Constructors/Destructors
  Stage(std::string n);

  //Interface functions
  void initialize(); //Initialize the contents of this stage to empty with NOP
  void display(); //Display the contents of this stage: name, opcode, operands

  //Utility functions
  int littoi(std::string literal);
  bool isAllValid(); //For this instr are all sources valid or no sources

}; //class Stage
#endif