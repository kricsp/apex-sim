/* FILE INFORMATION
File:     iq.cpp
Authors:  Matthew Cole <mcole8@binghamton.edu>
          Brian Gracin <bgracin1@binghamton.edu>
Description: Contains the IQ class, which simulates the operation of a Instruction Queue.
*/

#include <iostream>
#include "iq.h"

IQ::IQ(){
	this->initialize();
}

IQ::~IQ(){

}

bool IQ::isEmpty(){
	return issue_queue.empty();
}

//Display the contents of the IQ
//Each row is cycle#, opcode of the contained stage
void IQ::display(){
	std::cout << "Cycle : Opcode";
	for (auto e : issue_queue){
		std::cout << e.c << " : " << e.opcode << std::endl;
	}
}

//Initialize the IQ to empty state
void IQ::initialize(){
	issue_queue.clear();
}

//Dispatch an instruction to IQ 
void IQ::dispatchInst(Stage &stage){
	this->issue_queue.push_back(stage);
}

//Forward the value of an R reg that was computed from an FU
//If an instruction is found to have an R reg that is being forwarded
//Set value and valid bit for associated operand fields
void IQ::updateSrc(std::string reg, int val){
	if (issue_queue.size() > 0){
		for (auto& e : issue_queue){

			//Handle forward to arithmatic instructions in IQ
			if(e.opcode == "ADD" ||
				e.opcode == "SUB" ||
				e.opcode == "MUL" ||
				e.opcode == "AND" ||
				e.opcode == "OR" ||
				e.opcode == "EX-OR"){
				if(e.operands.at(1) == reg){
					e.values.at(1) = val;
					e.valids.at(1) = true;
				}
				if(e.operands.at(2) == reg){
					e.values.at(2) = val;
					e.valids.at(2) = true;
				}
			}

			//Handle forward to LOAD instruction in IQ
			if(e.opcode == "LOAD"){
				if(e.operands.at(1) == reg){
					e.values.at(1) = val;
					e.valids.at(1) = true;
				}
			}

			//Handle forward to branch instructions in IQ
			if(e.opcode == "BAL" ||
				e.opcode == "JUMP"){
				if(e.operands.at(0) == reg){
					e.values.at(0) = val;
					e.valids.at(0) = true;
				}
			}

			//Handle forward to STORE instructions in IQ
			if(e.opcode == "STORE"){
				if(e.operands.at(0) == reg){
					e.values.at(0) = val;
					e.valids.at(0) = true;
				}
				if(e.operands.at(1) == reg){
					e.values.at(1) = val;
					e.valids.at(1) = true;
				}
			}
		}
	}
}

// Flush all entries in the IQ with whose cycle time stamp
// is >= specified time stamp (used when branch is taken)
void IQ::flush(int cycle){
	// ASSUMPTION: the entries in the IQ and ROB are
	// sorted at all times by their timestamp of creation (c)

	// Point an iterator at the start of the IQ
	std::deque<Stage>::iterator it = issue_queue.begin();

	// Traverse until encountering an entry
	// whose cycle timestamp indicates it must be flushed
	while(it->c < cycle){
		++it;
	}

	// flush the elements from the current iterator to end:
	issue_queue.erase(it, issue_queue.end());
}