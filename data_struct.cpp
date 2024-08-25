#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <algorithm>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include <cmath>
#include <utime.h>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>

using std::cin, std::cout, std::endl;
using std::string;

class Index{
	private : 
		string file_name;
		string fileHash;
		unsigned int file_size, block_size, no_blocks;
		struct stat file_stat;
		string checksum;
	public :
		void select_component(bool);
		/* if the boolean input is 1 then its folder search 
			else its a file search */
		std::string computeHash(const std::vector<char>& data);
		void extract_origInfo(std::filesystem::path);
		std::filesystem::path generate_indexfile(std::filesystem::path);
		void read_indexfile(std::filesystem::path);
		void fragmentation(std::filesystem::path);
		void defragmentation(std::filesystem::path);

};

class Fragment : public Index {
	private:
		unsigned int fragment_size, total_blocks;
		int prev_frag_no, next_frag_no;
		unsigned int fragment_no;
		string frag_no;
		string data;
	public :
	Fragment(){}
	Fragment(unsigned int, unsigned int); 
	void packaging(std::vector<std::string> *fragment, std::filesystem::path);
	void serialization(std::ofstream &out, std::string frag_data);
	void deserialization(std::ifstream &in);
	
	unsigned int get_fragment_no() const { return fragment_no; }
	unsigned int get_fragment_size() const { return fragment_size; }
    int get_prev_frag_no() const { return prev_frag_no; }
	int get_next_frag_no() const { return next_frag_no; }
	std::string get_data() const { return data; }
};


class Defragment {
	private :
		Fragment defr_file;
};
