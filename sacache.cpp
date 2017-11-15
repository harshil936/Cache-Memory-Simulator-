#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

struct CacheLine {
	private:
		int tag = -1;
		int lru = 0;
		char data[8] = {0};
		bool dirty = false;

	public:
		int getTag(){
			return tag;
		}

		void setTag(int newtag){
			tag = newtag;
		}

		int isDirty(){
			return dirty;
		}

		void setDirty(){
			dirty = true;
		}

		void clearDirty(){
			dirty = false;
		}

		char getDataAt(int i){
			return data[i];
		}

		void setDataAt(int i, char val){
			data[i] = val;
		}

		void incrementLRU(){
			lru = lru + 1;
		}

		int getLRU(){
			return lru;
		}

		int resetLRU(){
			lru = 0;
		}

};

int main(int argc, char** argv) {
	
	const int SizeAtEntry = 8; 
	const int SizeOfLine = 4;
	int temp;
	int address;
	int rw;
	int data;
	int tag;
	int pos;
	int path = 6;
	bool hit;
	CacheLine line[SizeAtEntry][path];
	char mem[0xFFFF];
	ifstream file(argv[1]);

	while (file >> hex >> address >> rw >> data) //go into the while if we have all the input
	{
		pos = (address >> 2) % SizeAtEntry;
		tag = address >> 3;

		for (int i = 0; i < path; i++) line[pos][i].incrementLRU();

		for (int i = 0; i < path; i++) {
			if (line[pos][i].getTag() == tag) {
				line[pos][i].resetLRU();// reset the lru value
				break;
			}
		} // if it reaches all the way to the end, it didn't hit anything

		hit = !(temp == path);

		if (!hit) {	// you didn't find it, so now you want to find the LRU
			int high = 0;
			temp = 0;
			for (int i = 0; i < path; i++) {
				if (high > line[pos][i].getLRU()) {
					high = line[pos][i].getLRU();
					temp = i;
				}
			}

			if (line[pos][temp].isDirty()) { //if it's not in the cache
				for (int i = 0; i < SizeOfLine; i++) {
						int ind = line[pos][temp].getTag() * SizeOfLine;
						mem[ind + i] = line[pos][temp].getDataAt(i);
					} // go inside the memory and store what's in the cache into the memory
				}// if there's something in memory
			for (int i = 0; i < SizeOfLine; i++) {
				int ind = tag * SizeOfLine;
				line[pos][temp].setDataAt(i, mem[ind + i]);
			}
			line[pos][temp].setTag(tag);
			line[pos][temp].clearDirty(); // since you replaced it, you now say it's not dirty
		}

		if (rw) {
			int index = address & 3;
			line[pos][temp].setDataAt(index, data);
			line[pos][temp].setDirty();
		}//writing

		else {
			int index = address & 3;
			cout << setw(2);
			cout << hex;
			cout << uppercase;
			cout << setfill('0');
			cout << (((int)(line[pos][temp].getDataAt(index))) & 0xFF);
			cout << " " << hit << " ";
			cout << line[pos][temp].isDirty() << endl;
		}

	}

	file.close(); 
	return 0;
} 