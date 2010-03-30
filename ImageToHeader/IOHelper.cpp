// IOHelper.cpp ~ by David Martin (6836-0922-86)                                                              

#include <bitset>
#include <cmath>
#include <iostream>
#include <fstream>
#include <map>
#include "IOHelper.h"

using namespace std;

/////////////////////////////////
///// Class Implementations /////
/////////////////////////////////

///// Intput File Stream
InputFileStream::InputFileStream(const char filePath[])
: p_file(NULL), m_fileOpen(false), m_filePath(filePath), m_numBytes(0) {
	m_fileOpen = bool(p_file=fopen(m_filePath.c_str(),"r"));
	if ( !m_fileOpen )
		cout << "ERROR: Could not open file" << endl;
	else {
		// The following portion of the code, which determines the input file size
		// was inspired by http://www.cplusplus.com/doc/tutorial/files/
		ifstream tmpFile (m_filePath.c_str(), ios::in|ios::binary|ios::ate);
		if (tmpFile.is_open()) {
			m_numBytes = tmpFile.tellg();
			tmpFile.close();
		}
	}
}

InputFileStream::~InputFileStream() {
	if ( p_file != NULL )
		fclose(p_file);
	p_file = NULL;
}

// Accessor Functions
bool InputFileStream::isOpen() {
	// The purpose of the isOpen function is to denote whether or not 
	// the file was able to be opened when the class was first instantiated
	// or if the file has been read to its end.
	return m_fileOpen;
}
long int InputFileStream::getNumBytes() { return m_numBytes; }

unsigned char* InputFileStream::getNextByte() {
	// This function return a pointer to the next byte in the file
	// and returns the NULL pointer if the file cannot be opened or has ended.
	
	// The position placeholder code of the following if statement was inspired by
	// http://www.cplusplus.com/reference/clibrary/cstdio/fgetpos/
	if ( m_fileOpen && !feof(p_file)) {         // If the file and be opened and has not reached the end
		fpos_t posCurrent;                      //  - create a position placeholder
		fgetpos(p_file,&posCurrent);            //  - note the current file position 
		unsigned char* val = new unsigned char; //  - create a pointer to a new unsigned char
		*val = getc(p_file);                    //  - read in the value from the file
		
		if ( posCurrent.__pos < m_numBytes ) {            // If the current position is less than the number of bytes in the file
			m_fileOpen = !feof(p_file);             //  - set the fileOpen flag noting if the file has ended
			return val;                             //  - return the value obtained from 
		}
	}
	return NULL;                                // Else return NULL
}

unsigned char* InputFileStream::getByte(long int position) {
	// This function reads a byte at a particular position or offset from the beginning of the file.
	// It keeps file position state and returns with the file position as it was when the function was called.
	
	// The following code, which reads in a byte at a particular location of a file
	// was inspired by http://www.cplusplus.com/reference/clibrary/cstdio/fseek/
	if (m_fileOpen) {
		// Note the original position within the file before seeking
		fpos_t posOriginal;
		fgetpos(p_file,&posOriginal);
		
		if (position < m_numBytes) {
			// Seek to the desired position and read a byte
			fseek(p_file, position, SEEK_SET);
			unsigned char* val = new unsigned char;
			*val = fgetc(p_file);
			fseek(p_file, posOriginal.__pos, SEEK_SET);   // Seek back to the previous position and return
			return val;
		}
	}
	return NULL;
}


// Manipulator Functions
void InputFileStream::reset() {
	if ( m_fileOpen )
		fseek(p_file, 0, SEEK_SET);
}


///// Output File Stream
OutputFileStream::OutputFileStream(const char filePath[])
: p_file(NULL), m_filePath(filePath), m_fileOpen(false), m_numBytes(0) {
	m_fileOpen = bool(p_file=fopen(m_filePath.c_str(),"wb"));
	if ( !m_fileOpen )
		cout << "ERROR: Could not open file" << endl;
}

OutputFileStream::~OutputFileStream() {
	if (m_fileOpen)
		fclose(p_file);
}

// Accessor Functions
bool OutputFileStream::isOpen() { return m_fileOpen; }
long int OutputFileStream::getNumBytes() { return m_numBytes; }

// Manipulator Functions
bool OutputFileStream::writeByte(unsigned char* val) {
	if (m_fileOpen) {
		// If the file can be written to, write the value and return true;
		//fwrite(val, sizeof(unsigned char), 1, p_file);
		fputc(*val, p_file);
		m_numBytes++;
		return true;
	}
	
	cout << "ERROR!!! Tried to write but couldn't" << endl;
	// Else, return false.
	return false;
}


///// Byte File
ByteFile::ByteFile(const char filePath[])
: p_inputFileStream(NULL) {
	p_inputFileStream = new InputFileStream(filePath);
}

ByteFile::~ByteFile() {
	delete p_inputFileStream;
}

// Accessor Functions
InputFileStream* ByteFile::getFileStream() { return p_inputFileStream; }

long int ByteFile::getNumBytes() { return getFileStream()->getNumBytes(); }
