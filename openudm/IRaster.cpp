#include "IRaster.h"
#include "CSVToolkit.h"

#include <limits>
#include <cstring> // for memcpy
#include <algorithm>

using namespace std;

IRaster::IRaster(void)
{
	//header data
	ncols = 0;
	nrows = 0;
	xllcorner = 0.0;
	yllcorner = 0.0;
	cellsize = 100.0;
	NODATA_value = -9999;	
}


IRaster::~IRaster(void)
{
}

void IRaster::Setup(int ncols, int nrows, int init) {
	
	this->ncols = ncols;
	this->nrows = nrows;

	//Allocate memory for dynamic array using ncols and nrows from header	
	data = new int*[nrows];
	for(int i = 0; i < nrows; ++i) {
		data[i] = new int[ncols];
	}

	//initialise data
	for (int r = 0; r != nrows; ++r) {
		for (int c = 0; c != ncols; ++c) {
			data[r][c] = init;
		}
	}	
}

void IRaster::Setup(std::string ipfile) {


	//declare an ifstream object
	//ifstream ipfileHeader ("testRaster.asc");
	ifstream ipfileHeader(ipfile);

	//declare a string object
	string line, read, value;

	//boolean header complete
	bool headerComplete = false;	

	//check the file opened OK
	if (ipfileHeader.is_open())
	{
		//cout << "Reading Header..." << endl << endl;

		while ( ipfileHeader.good() && !headerComplete)
		{
			
			stringstream lineStream;
			
			//read a line from the file into string line
			getline( ipfileHeader, line );
			//and then into linestream
			lineStream << line;
			//and then out to string read
			lineStream >> read;

			if(read == "ncols") {
				//cout << "ncols data detected.." << endl;
				lineStream >> value;
				ncols = GetIntFromString(value);
				//cout << "ncols = " << ncols << endl;
			} 

			if(read == "nrows") {
				//cout << "nrows data detected.." << endl;
				lineStream >> value;
				nrows = GetIntFromString(value);
				//cout << "nrows = " << nrows << endl;
			} 

			if(read == "xllcorner") {
				//cout << "xllcorner data detected.." << endl;
				lineStream >> value;
				xllcorner = GetDoubleFromString(value);
				//cout << "xllcorner = " << xllcorner << endl;
			} 

			if(read == "yllcorner") {
				//cout << "yllcorner data detected.." << endl;
				lineStream >> value;
				yllcorner = GetDoubleFromString(value);
				//cout << "yllcorner = " << yllcorner << endl;
			} 

			if(read == "cellsize") {
				//cout << "cellsize data detected.." << endl;
				lineStream >> value;
				cellsize = GetDoubleFromString(value);
				//cout << "cellsize = " << cellsize << endl;
			} 

			if(read == "NODATA_value") {
				//cout << "NODATA_value data detected.." << endl;
				lineStream >> value;
				NODATA_value = GetIntFromString(value);
				//cout << "NODATA_value = " << NODATA_value << endl << endl;
				headerComplete = true;
			} 					
		}
	}
	ipfileHeader.close();	

	//Allocate memory for dynamic array using ncols and nrows from header	
	data = new int*[nrows];
	for(int i = 0; i < nrows; ++i) {
		data[i] = new int[ncols];
	}

	//initialise data
	for (int r = 0; r != nrows; ++r) {
		for (int c = 0; c != ncols; ++c) {
			data[r][c] = 0;
		}
	}	
}

void IRaster::Read(std::string ipfile) {

	ifstream ipfileData(ipfile);
	int rowNum = 0;
	bool readArray = false;
	//declare string objects
	string line, read, value;

	//check the file opened OK
	if (ipfileData.is_open())
	{
		cout << "Reading Data..." << endl << endl;

		while ( ipfileData.good() )
		{
			if(!readArray) {
				stringstream lineStream;
			
				//read a line from the file into string line
				getline( ipfileData, line );
				//and then into linestream
				lineStream << line;
				//and then out to string read
				lineStream >> read;

				//skip header this time through but switch to readArray after final header line
				if(read == "NODATA_value") {	
					//cout << "skipping header.." << endl;
					readArray = true;
					line.clear();
				} 
			} else
			{
				//cout << "HERE?" << endl;

				stringstream lineStream;
			
				//read a line from the file into string line
				getline( ipfileData, line );

				if (!line.empty()) {
					//and then into linestream
					lineStream << line;

					for (int i = 0; i != ncols; ++i) {
						lineStream >> value;
						//if(rowNum <= nrows) {
						data[rowNum][i] = GetIntFromString(value);
						//cout << "READ" << endl;
						//}	

					}
					++rowNum;
				}
			}			
		}		
	}
	ipfileData.close();
}

void IRaster::Write(std::string writefile) {

	//create an ofstream object
	ofstream opfile(writefile);

	//check the file opened OK
	if (opfile.is_open()) {
		
		//write header
		opfile << "ncols" << " " << ncols << "\n";
		opfile << "nrows" << " " << nrows << "\n";
		opfile << "xllcorner" << " " << xllcorner << "\n";
		opfile << "yllcorner" << " " << yllcorner << "\n";
		opfile << "cellsize" << " " << cellsize << "\n";
		opfile << "NODATA_value" << " " << NODATA_value << "\n";

		//write raster data
		for(int r = 0; r != nrows; ++r) {
			for(int c = 0; c != ncols; ++c) {
				opfile << data[r][c] << " ";
			}
			if(r < nrows-1) {
				opfile << "\n";
			}
		}			
			
		//close opfile
		opfile.close();		
	}
	else {
		cout << "Unable to open output file"; 
	}	
}

void IRaster::FromPGBinary(std::string binData) {

	int hdrLen = 19;	//19byte header
	int ftrLen = 2;		//2byte footer
	//int bufDbl = 14;	//6bytes of padding followed by 8bytes of double precision data
	int bufInt = 10;	//6bytes of padding followed by 4bytes of integer data

	streampos size;
	char* buffer;
	int bufSize;

	ifstream file(binData, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		//get size for read operation - not including header and footer
		size = file.tellg();
		size -= (hdrLen + ftrLen);
		bufSize = (int)size;

		//setup read buffer
		buffer = new char[bufSize];

		//skip header the read entire file up to the footer
		file.seekg(hdrLen, ios::beg);
		file.read(buffer, size);
		file.close();

		//check size and number of values to be read
		//cout << "size of file = " << size << " bytes" << endl;
		//int numValues = bufSize / bufInt;
		//cout << "number of values = " << numValues << endl;

		//buffer - each buffered integer is 10 bytes comprising..
		//0->1   - 2 byte count of numFields
		//2->5   - 4 byte word followed by that many bytes of data
		//6->9   - 4 bytes of binary data representing an integer (big endian)

		//set index to 1D buffer
		int vNum = 0;

		for (int r = 0; r != nrows; ++r) {
			for (int c = 0; c != ncols; ++c) {

				//reverse bytes to move from big to little endian			
				//std::reverse(&buffer[vNum*bufDbl + 6], &buffer[vNum*bufDbl + 14]);
				std::reverse(&buffer[vNum*bufInt + 6], &buffer[vNum*bufInt + 10]);

				//copy bytes from charArray into dblArray to convert type			
				//std::memcpy(&data[r][c], &buffer[vNum*bufDbl + 6], sizeof(double));
				std::memcpy(&data[r][c], &buffer[vNum*bufInt + 6], sizeof(int));

				//increment vNum index to buffer
				++vNum;
			}
		}

		//free char array
		delete[] buffer;
	}
	else cout << "Unable to open file";
}

void IRaster::ToPGBinary(std::string binData) {

	///int bufDbl = 14;
	int bufInt = 10;

	//header
	int hdrLen = 19;
	char* hdrBuf;
	hdrBuf = new char[hdrLen];

	//footer
	int ftrLen = 2;
	char* ftrBuf;
	ftrBuf = new char[ftrLen];

	//padding
	int padLen = 6;
	char* padBuf;
	padBuf = new char[padLen];

	char* buffer;
	int bufferSize = (nrows* ncols * bufInt) + hdrLen + ftrLen;
	buffer = new char[bufferSize];

	ifstream hdrFile("hdr.bin", ios::in | ios::binary | ios::ate);
	if (hdrFile.is_open()) {
		//read header
		hdrFile.seekg(0, ios::beg);
		hdrFile.read(hdrBuf, hdrLen);
		hdrFile.close();
	}
	else cout << "unable to open header file" << endl;

	ifstream ftrFile("ftr.bin", ios::in | ios::binary | ios::ate);
	if (ftrFile.is_open()) {
		//read header
		ftrFile.seekg(0, ios::beg);
		ftrFile.read(ftrBuf, ftrLen);
		ftrFile.close();
	}
	else cout << "unable to open footer file" << endl;

	ifstream padFile("i_pad.bin", ios::in | ios::binary | ios::ate);
	if (padFile.is_open()) {
		//read header
		padFile.seekg(0, ios::beg);
		padFile.read(padBuf, padLen);
		padFile.close();
	}
	else cout << "unable to open padding file" << endl;

	//copy header to buffer
	std::memcpy(buffer, hdrBuf, hdrLen);

	//copy integer values to buffer	

	//buffer - each buffered integer is 10 bytes comprising..
	//0->1   - 2 byte count of numFields
	//2->5   - 4 byte word followed by that many bytes of data
	//6->9   - 4 bytes of binary data representing an integer (big endian)

	//set index to 1D buffer
	int vNum = 0;

	for (int r = 0; r != nrows; ++r) {
		for (int c = 0; c != ncols; ++c) {

			//copy padding bytes into buffer			
			//std::memcpy(&buffer[vNum * bufDbl + hdrLen], padBuf, padLen);
			std::memcpy(&buffer[vNum * bufInt + hdrLen], padBuf, padLen);

			//copy bytes from dblArray into charArray to convert type			
			//std::memcpy(&buffer[vNum * bufDbl + 6 + hdrLen], &data[r][c], sizeof(double));
			std::memcpy(&buffer[vNum * bufInt + 6 + hdrLen], &data[r][c], sizeof(int));

			//reverse bytes to move from little to big endian			
			//std::reverse(&buffer[vNum*bufDbl + 6 + hdrLen], &buffer[vNum*bufDbl + 14 + hdrLen]);
			std::reverse(&buffer[vNum*bufInt + 6 + hdrLen], &buffer[vNum*bufInt + 10 + hdrLen]);

			//increment vNum index to buffer
			++vNum;
		}
	}

	//copy footer to buffer
	//std::memcpy(&buffer[(nrows * ncols * bufDbl) + hdrLen], ftrBuf, ftrLen);
	std::memcpy(&buffer[(nrows * ncols * bufInt) + hdrLen], ftrBuf, ftrLen);

	//write buffer to file
	ofstream out(binData, ios::out | ios::binary);
	if (out.is_open())
	{
		out.seekp(0, ios::beg);
		out.write(buffer, bufferSize);
		out.close();
	}
	else cout << "Unable to open output file";
}

void IRaster::ToPGBinary(std::string hdrPadFtrPath, std::string binData) {

	///int bufDbl = 14;
	int bufInt = 10;

	//header
	int hdrLen = 19;
	char* hdrBuf;
	hdrBuf = new char[hdrLen];

	//footer
	int ftrLen = 2;
	char* ftrBuf;
	ftrBuf = new char[ftrLen];

	//padding
	int padLen = 6;
	char* padBuf;
	padBuf = new char[padLen];

	char* buffer;
	int bufferSize = (nrows* ncols * bufInt) + hdrLen + ftrLen;
	buffer = new char[bufferSize];

	std::string hdrPath = hdrPadFtrPath + "hdr.bin";

	//ifstream hdrFile("hdr.bin", ios::in | ios::binary | ios::ate);
	ifstream hdrFile(hdrPath, ios::in | ios::binary | ios::ate);
	if (hdrFile.is_open()) {
		//read header
		hdrFile.seekg(0, ios::beg);
		hdrFile.read(hdrBuf, hdrLen);
		hdrFile.close();
	}
	else cout << "unable to open header file" << endl;

	std::string ftrPath = hdrPadFtrPath + "ftr.bin";

	//ifstream ftrFile("ftr.bin", ios::in | ios::binary | ios::ate);
	ifstream ftrFile(ftrPath, ios::in | ios::binary | ios::ate);
	if (ftrFile.is_open()) {
		//read header
		ftrFile.seekg(0, ios::beg);
		ftrFile.read(ftrBuf, ftrLen);
		ftrFile.close();
	}
	else cout << "unable to open footer file" << endl;

	std::string padPath = hdrPadFtrPath + "i_pad.bin";

	//ifstream padFile("i_pad.bin", ios::in | ios::binary | ios::ate);
	ifstream padFile(padPath, ios::in | ios::binary | ios::ate);
	if (padFile.is_open()) {
		//read header
		padFile.seekg(0, ios::beg);
		padFile.read(padBuf, padLen);
		padFile.close();
	}
	else cout << "unable to open padding file" << endl;

	//copy header to buffer
	std::memcpy(buffer, hdrBuf, hdrLen);

	//copy integer values to buffer	

	//buffer - each buffered integer is 10 bytes comprising..
	//0->1   - 2 byte count of numFields
	//2->5   - 4 byte word followed by that many bytes of data
	//6->9   - 4 bytes of binary data representing an integer (big endian)

	//set index to 1D buffer
	int vNum = 0;

	for (int r = 0; r != nrows; ++r) {
		for (int c = 0; c != ncols; ++c) {

			//copy padding bytes into buffer			
			//std::memcpy(&buffer[vNum * bufDbl + hdrLen], padBuf, padLen);
			std::memcpy(&buffer[vNum * bufInt + hdrLen], padBuf, padLen);

			//copy bytes from dblArray into charArray to convert type			
			//std::memcpy(&buffer[vNum * bufDbl + 6 + hdrLen], &data[r][c], sizeof(double));
			std::memcpy(&buffer[vNum * bufInt + 6 + hdrLen], &data[r][c], sizeof(int));

			//reverse bytes to move from little to big endian			
			//std::reverse(&buffer[vNum*bufDbl + 6 + hdrLen], &buffer[vNum*bufDbl + 14 + hdrLen]);
			std::reverse(&buffer[vNum*bufInt + 6 + hdrLen], &buffer[vNum*bufInt + 10 + hdrLen]);

			//increment vNum index to buffer
			++vNum;
		}
	}

	//copy footer to buffer
	//std::memcpy(&buffer[(nrows * ncols * bufDbl) + hdrLen], ftrBuf, ftrLen);
	std::memcpy(&buffer[(nrows * ncols * bufInt) + hdrLen], ftrBuf, ftrLen);

	//write buffer to file
	ofstream out(binData, ios::out | ios::binary);
	if (out.is_open())
	{
		out.seekp(0, ios::beg);
		out.write(buffer, bufferSize);
		out.close();
	}
	else cout << "Unable to open output file";
}

void IRaster::Cleanup() {

	//Free memory from dynamic array
	for (int i = 0; i < nrows; ++i) {
		delete[] data[i];
	}
	delete[] data;

}

void IRaster::FromCSV(std::string csvFile) {	

	//determine number of raster cells
	int rasterSize = ncols * nrows;

	//setup string to hold values for all cells
	std::string* readStr;
	readStr = new std::string[rasterSize];

	//read from input csv file into single column
	ExtractCSV(csvFile, 1, 0, readStr);	

	//set 1 dimensional cell index
	int cellIndex = 0;

	//write 2d integer array from 1d string array 
	for (int r = 0; r != nrows; ++r) {
		for (int c = 0; c != ncols; ++c) {
			data[r][c] = GetIntFromString(readStr[cellIndex]);
			++cellIndex;
		}
	}

	//tidy up
	delete[] readStr;
}

void IRaster::ToCSV(std::string csvFile) {

	//create an ofstream object
	ofstream opfile(csvFile);

	//check the file opened OK
	if (opfile.is_open()) {

		//write header
		opfile << "c1" << "\n";		

		//write raster data
		for (int r = 0; r != nrows; ++r) {
			for (int c = 0; c != ncols; ++c) {
				
				opfile << data[r][c] << "\n";
			}			
		}

		//close opfile
		opfile.close();
	}
	else {
		cout << "Unable to open csv output file";
	}
}



