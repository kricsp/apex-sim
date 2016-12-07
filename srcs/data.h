/* FILE INFORMATION
File:     data.h
Authors:  Matthew Cole <mcole8@binghamton.edu>
          Brian Gracin <bgracin1@binghamton.edu>
Description: Header for data.cpp
*/

#ifndef DATA_H
#define DATA_H

#include <vector>

class Data
{
private:
	std::vector<int>  memory;

public:
	Data(); //initialize the memory with all positions set to zero

	~Data(); //Deconstructor

	void initialize();

  	//Read one byte-address from memory at <addr>
  	int readMem(int addr);

  	//Write one byte to memory at <addr>
  	void writeMem(int addr, int value);

  	//Display addressable contents of memory from a1 to a2 inclusive
  	void display(int a1, int a2);

};
#endif
