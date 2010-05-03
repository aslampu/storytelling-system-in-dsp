// IOHelper.h ~ by David Martin (6836-0922-86)

#ifndef IOHELPER_INCLUDED
#define IOHELPER_INCLUDED

#include <cstdio>
#include <cstring>
#include <list>
#include <map>

using namespace std;

/////////////////////////////
///// Class Definitions /////
/////////////////////////////

///// File Stream Classes
class OutputFileStream {
	// This class is designed to allow a file to be written to in multiple ways.
	//     writeByte() -> unsigned char
	//     --more to come--
public:
	OutputFileStream(const char filePath[]);
	~OutputFileStream();
	
	// Accessor Functions
	bool isOpen();
	long int getNumBytes();
	
	// Manipulator Functions
	bool writeByte(unsigned char* val);
private:
	FILE* p_file;
	string m_filePath;
	bool m_fileOpen;
	long int m_numBytes;
};

class InputFileStream {
	// This class is intended to allow stream-like access to a file with multiple read methods.
	//     getByte() -> unsigned char
	//     --more to come--
public:
	InputFileStream(const char filePath[]);
	~InputFileStream();
	
	// Accessor Functions
	bool isOpen();
	long int getNumBytes();
	unsigned char* getNextByte();
	unsigned char* getByte(long int position);
	
	// Manipulator Functions
	void reset();
private:
	FILE* p_file;
	bool m_fileOpen;
	string m_filePath;
	long int m_numBytes;
};

///// File Container Classes
class ByteFile {
public:
	ByteFile(const char filePath[]);
	~ByteFile();
	
	// Accessor Functions
	InputFileStream* getFileStream();
	long int getNumBytes();
private:
	InputFileStream* p_inputFileStream;
};

#endif // IOHELPER_INCLUDED