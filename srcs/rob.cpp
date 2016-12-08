/* FILE INFORMATION
File:     rob.cpp
Authors:  Matthew Cole <mcole8@binghamton.edu>
          Brian Gracin <bgracin1@binghamton.edu>
Description: Contains the ROB class, which simulates the operation of a Reorder Buffer.
*/

#include "rob.h"

ROB::ROB();

ROB::~ROB();

//Remove head from ROB and call registers commit function to update backend table and free list
void ROB::commit(Registers &reg){
	std::string pReg;
	pReg = this->reorder_buffer.front().operands.at(0);
	reg.commit(pReg);
}

//Add a Stage instance to ROB
void ROB::addStage(Stage &stage){
	this->reorder_buffer.push(stage);
}

//Given a cycle value gets cycle value of current head and returns if they are equal (==)
bool ROB::headMatch(int cycle){
	int size = 0;
	int headCycle = 0;

	size = this->reorder_buffer.size();

	if(size > 0){
		headCycle = this->reorder_buffer.front().cycle;
	}

	if(headCycle == cycle){
		return true;
	}

	return false;
}