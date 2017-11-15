
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

class CacheLine {

private:
	int tag = -1;
	bool dirty = false;
	char data[8] = { 0 };

public:

	int getTag() {
		return tag;
	} //getTag

	void setTag(int newtag) {
		tag = newtag;
	} //setTag

	int isDirty() {
		return dirty;
	} // isDirty

	void setDirty() {
		dirty = true;
	} //setDirty

	void clearDirty() {
		dirty = false;
	} //cleanDirty

	char getDataAt(int i) {
		return data[i];
	} //getDataAt

	void setDataAt(int i, char val) {
		data[i] = val;
	} //setDataAt

};

int main(int argc, char** argv) {

	CacheLine line[32];
	int line_size = 8; // given 
	int entry_size = 32; // given
	int tag;
	int address;
	int data;
	int rw;
	int pos;
	int hit;
	char mem[0xFFFF];
	ifstream file(argv[1]); // get file from input

	while (file >> hex >> address >> rw >> data) {
		int pos = (address >> 3) % entry_size; // position of block in input/memory
		int tag = address >> 5; // tag from input line

		hit = line[pos].getTag() == tag;

		if (!hit) { // not in cache

			if (line[pos].isDirty()) { // if there's something in memory
				for (int i = 0; i < line_size; i++) { // hold input data in memory 
					mem[line[pos].getTag() * line_size + i] = line[pos].getDataAt(i);
				}
			}

			for (int i = 0; i < line_size; i++) { // set memory at input index to cache
				line[pos].setDataAt(i, mem[tag * line_size + i]);
			}

			line[pos].setTag(tag); // tags are now consistent
			line[pos].clearDirty(); // clear dirty bit (cache and memory now match)
		}

		if (rw) {

			line[pos].setDataAt(address & 7, data);
			line[pos].setDirty(); // set dirty bit

		}
		else {
			int index = address & (7);
			cout << setw(2);
			cout << hex;
			cout << uppercase;
			cout << setfill('0');
			cout << (((int)(line[pos].getDataAt(index))) & 0xFF);
			cout << ' ' << hit << ' ' << line[pos].isDirty() << endl;
		}
	}

	file.close();
	return 0;

}