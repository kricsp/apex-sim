/* FILE INFORMATION
File:     iq.cpp
Authors:  Matthew Cole <mcole8@binghamton.edu>
          Brian Gracin <bgracin1@binghamton.edu>
Description: Contains the IQ class, which simulates the operation of a Instruction Queue.
*/

#include <iostream>
#include "iq.h"
#include "cpu.h"

IQ::IQ(){
	this->initialize();
}

IQ::~IQ(){}

// Return true if the IQ is empty,
// Return false otherwise
// (Delegates to stl::queue::empty())
bool IQ::isEmpty(){
	return issue_queue.empty();
}

//Display the contents of the IQ
//Each entry is a Stage, so we delegate the display logic
void IQ::display(){
	std::cout << "Head" << std::endl;
	for (auto e : issue_queue){
		e.display();
	}
	std::cout << "Tail" << std::endl;
}

//Initialize the IQ to empty state
void IQ::initialize(){
	issue_queue.clear();
}

//Dispatch an instruction to IQ
bool IQ::dispatchInst(Stage &stage){
	if (this->issue_queue.size() < this->max_size){
		stage.empty = false;

		if (stage.opcode == "ADD" ||
			stage.opcode == "SUB" ||
			stage.opcode == "MUL" ||
			stage.opcode == "AND" ||
			stage.opcode == "OR" ||
			stage.opcode == "EX-OR"){
			if (stage.valids.at(1) && stage.valids.at(2)){
				stage.ready = true;
			}
		}
		else if (stage.opcode == "MOVC" ||
			stage.opcode == "BZ" ||
			stage.opcode == "BNZ"){
			stage.ready = true;
		}
		else if (stage.opcode == "STORE"){
			if (stage.valids.at(1))
				stage.ready = true;
		}
		else if (stage.opcode == "LOAD"){
			if (stage.valids.at(1)){
				stage.ready = true;
			}
		}
		else if (stage.opcode == "BAL" ||
			stage.opcode == "JUMP"){
			if (stage.valids.at(0))
				stage.ready = true;
		}

		this->issue_queue.push_back(stage);
		return true;
	}

	return false;
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

bool IQ::issue(Stage& ALU, Stage& MUL, Stage& LSFU, Stage& B){
	int numIssued = 0;
	bool hitArith = false;
	bool advSuccess = false;
	for (auto i = this->issue_queue.begin(); i != this->issue_queue.end();){
		if (i->isReady()){

			//Check if no arithmatic instructions prior to conditional branch
			if(i->opcode == "ADD" ||
				i->opcode == "SUB" ||
				i->opcode == "AND" ||
				i->opcode == "OR" ||
				i->opcode == "EX-OR" ||
				i->opcode == "MUL"){
				hitArith = true;
			}

			//Arithmetic Opcodes
			if(i->opcode == "ADD" ||
				i->opcode == "SUB" ||
				i->opcode == "AND" ||
				i->opcode == "OR" ||
				i->opcode == "EX-OR" ||
				i->opcode == "MOVC"){
				advSuccess = i->advance(ALU);

				if ( advSuccess ){
					this->issue_queue.erase(i);
					numIssued++;
				}
			}
			//Multiplication Opcode
			if(i->opcode == "MUL"){
				advSuccess = i->advance(MUL);

				if ( advSuccess ){
					this->issue_queue.erase(i);
					numIssued++;
				}
			}
			//Memory Access Opcodes
			if(i->opcode == "LOAD" || i->opcode == "STORE"){
				advSuccess = i->advance(LSFU);

				if ( advSuccess ){
					this->issue_queue.erase(i);
					numIssued++;
				}
			}
			//Unconditional Branches Opcodes
			if(i->opcode == "BAL" ||
				i->opcode == "JUMP"){
				advSuccess = i->advance(B);

				if ( advSuccess ){
					this->issue_queue.erase(i);
					numIssued++;
				}
			}

			//Conditional Branches Opcodes
			if((i->opcode == "BZ" ||
				i->opcode == "BNZ") 
				&& !hitArith){
				advSuccess = i->advance(B);

				if ( advSuccess ){
					this->issue_queue.erase(i);
					numIssued++;
				}
			}
		}
		else{
			++i;
		} //this entry is not ready
		if (numIssued > 2){
			return true;
		}

		advSuccess = false;
	} //END for each entry in issue_queue

	//Failed to issue instruction
	return false;
} //END issue() function

// Flush all entries in the IQ with whose cycle time stamp
// is >= specified time stamp (used when branch is taken)
// ASSUMPTION: the entries in the IQ and ROB are
// 						sorted at all times by their timestamp of creation (c)
void IQ::flush(int cycle, Registers &rf){
	// Point an iterator at the start of the IQ
	std::deque<Stage>::iterator it = issue_queue.begin();

	// Traverse until encountering an entry
	// whose cycle timestamp indicates it must be flushed
	while(it->c < cycle){
		++it;
	}

	// Flush the elements from the current iterator to end:
	while (it != issue_queue.end()){
			// Deallocate physical registers by delegation
			it->flush(cycle, rf);

			// Erase this entry in the IQ
			issue_queue.erase(it);

			// Advance the iterator
			++it;
	}
}

// Given an opcode string,
// Return true if an entry in the IQ has that opcode
// Return false otherwise
bool IQ::hasEntryWithOpcode(std::string oc){
	for (auto entry : issue_queue){
		if (entry.opcode == oc){
			return true;
		}
	}

	return false;
}
