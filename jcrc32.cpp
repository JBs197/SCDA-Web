#include "jcrc32.h"

void JCRC32::init(string seed)
{
	vector<int> viTemp(seed.size());
	set<int> setPower;
	string temp;
	for (int ii = 0; ii < viTemp.size(); ii++)
	{
		viTemp[ii] = (int)seed[ii];
		viTemp[ii] = viTemp[ii] % 32;
		while (setPower.count(viTemp[ii])) {
			viTemp[ii]++;
			if (viTemp[ii] >= 32) { viTemp[ii] = 0; }
		}
		setPower.emplace(viTemp[ii]);
	}
	jf.quicksort(viTemp, 0, viTemp.size() - 1);
	crcTable = jf.makeCRC32(viTemp);
}
unsigned JCRC32::getChecksum(string& data)
{
	vector<unsigned char> vucData(data.size());
	for (int ii = 0; ii < data.size(); ii++) {
		vucData[ii] = (unsigned char)data[ii];
	}
	unsigned crc = getChecksum(vucData);
	return crc;
}
unsigned JCRC32::getChecksum(vector<unsigned char>& data)
{
	if (crcTable.size() < 1) { jf.err("No init-jcrc32.getChecksum"); }
	unsigned crc = crcTable[crcTable.size() - 1];
	size_t index = 0;
	size_t length = data.size();
	while (length--) {
		crc = crcTable[(crc ^ data[index]) & 255] ^ (crc >> 8);
		index++;
	}
	crc ^= 0xffffffffL;
	return crc;
}
