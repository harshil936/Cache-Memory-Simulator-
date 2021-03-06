#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

class Cache {

public:
	int tag, lru;
	char data[8];
	bool dirty;
	Cache() :tag(-1), lru(0), dirty(false)
	{
		for (int i = 0; i < 4; i++)
			data[i] = 0;
	} // initializes the caches tag as -1 (because it starts at 0 -> 2^8) 
};

void write(Cache line[8][6], int address, int index, int data, int num) {
	line[index][num].data[address & 3] = data;
	line[index][num].dirty = true;

}//write


void read(int hit, bool dirty, Cache line[8][6], int address, int index, int num) {

	cout << setw(2) << hex << uppercase << setfill('0')
		<< (((int)(line[index][num].data[address & 3])) & 0xFF)
		<< ' ' << hit << ' ' << dirty << endl;
}//read

 /*
 Implement 256 byte Direct-mapped cache
 Line size = 8 byte
 # of entries = 32
 For the DM implementation of Cache we have two cases
 Case 1: If it hits (means find the data in cache), then only operations that we can do is read and write from that cache.
 Case 2: If it miss (means cannot find the data in cache), then we have to goto Main memory where we will have 2 possibilities:
 (i) Now if there is a dirty bit then read from Main memory to cache.
 (ii) If it's not dirty,

 */
int main(int argc, char** argv) {
	const int linesize = 4, entry_size = 8, way = 6;
	int temp;
	int address, readwrite, data, tag, index;
	bool hit, dirty;
	Cache line[entry_size][way];
	char memory[0xFFFF];
	ifstream file(argv[1]);

	while (file >> hex >> address >> readwrite >> data) {
		index = (address / linesize) % entry_size;
		tag = (address) / entry_size;

		for (int i = 0; i < way; i++) {
			line[index][i].lru++;
		}//increment lru 
		for (int i = 0; i < way; i++) {
			if (line[index][i].tag == tag) {
				line[index][i].lru = 0; // reset the lru value
				break;
			}//if
		}//for -> check to see if it reaches all the way to the end 
		 // if it reaches all the way to the end, it didn't hit anything
		if (temp == way) {
			hit = false;
		} // no hit
		else {
			hit = true;
		} // hit

		dirty = line[index][temp].dirty; // taking the case of if it's dirty or not before you go in.

		if (hit == false) {
			// you didn't find it, so now you want to find the LRU
			int high = 0;
			temp = 0;
			for (int i = 0; i < way; i++) {
				if (high> line[index][i].lru) {
					high = line[index][i].lru;
					temp = i;
				}//if
			}//for
			dirty = line[index][temp].dirty;
			if (line[index][temp].dirty) {
				for (int i = 0; i < linesize; i++) {
					(&memory[line[index][temp].tag * linesize])[i] = line[index][temp].data[i];
				} // go inside the memory and store what's in the cache into the memory
			}// if there's something in memory
			for (int i = 0; i < linesize; i++) {
				line[index][temp].data[i] = (&memory[tag * linesize])[i];
			}//for
			line[index][temp].tag = tag;
			line[index][temp].dirty = false; // since you replaced it, you now say it's not dirty
		} //if it's not in the cache

		if (readwrite) {
			write(line, address, index, data, temp);
		}//writing
		else {
			read(hit, dirty, line, address, index, temp);
		}// reading

	}//while

	file.close(); //closing file
	return 0;
} // main