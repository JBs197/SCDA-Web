#pragma once
#include "jfunc.h"

using namespace std;

class JCRC32
{
	vector<unsigned> crcTable;
	JFUNC jf;

public:
	JCRC32() {}
	~JCRC32() {}

	void init(string seed);
	unsigned getChecksum(string& data);
	unsigned getChecksum(vector<unsigned char>& data);
};

