

// #include <string.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "laneLines.hpp"


using namespace std;


bool isValidImageFileName(char* fileName)
{

	char* ending = strrchr(fileName, '.');

	if (!ending) { return false; }

	//todo: convert to lowercase
	// char* lowercase;

	return (strcmp(ending, ".jpg") == 0) || //JPEG
		(strcmp(ending, ".jpeg") == 0) ||
		(strcmp(ending, ".jpe") == 0) ||
		//PNG
		(strcmp(ending, ".png") == 0) ||
		//TIFF
		(strcmp(ending, ".tiff") == 0) ||
		//JPEG 2000
		(strcmp(ending, ".jp2") == 0) ||
		//Windows Bitmap
		(strcmp(ending, ".bmp") == 0) ||
		//Portable image formats
		(strcmp(ending, ".pbm") == 0) ||
		(strcmp(ending, ".pgm") == 0) ||
		(strcmp(ending, ".ppm") == 0) ||
		//Sun raster
		(strcmp(ending, ".sr") == 0) ||
		(strcmp(ending, ".ras") == 0);

}

bool isValidVideoInputFileName(char* fileName)
{
	char* ending = strrchr(fileName, '.');

	if (!ending) { return false; }

	//todo: convert to lowercase
	// char* lowercase;

	return (strcmp(ending, ".mp4") == 0) || (strcmp(ending, ".avi") == 0);
}

bool isValidVideoOutputFileName(char* fileName)
{
	char* ending = strrchr(fileName, '.');

	if (!ending) { return false; }

	//todo: convert to lowercase
	// char* lowercase;

	return (strcmp(ending, ".avi") == 0);
}

void printHelp()
{
	cout << "Usage: ./executable [options]" << endl << endl;
	cout << "Options:" << endl;
	cout << "  -h\t\t\t\tShow this help message and exit." << endl;
	cout << "  -i INPUT_FILE\t\tInput video/image file." << endl;
	cout << "  -o OUTPUT_FILE\t\tOutput video/image file." << endl;
	cout << "  -I\t\t\t\tAnnotate image (defaults to annotating video)" << endl;
}


//todo: had to remove const from argv for this to compile with getopt, probably should make changes elsewhere
int main(int argc, char *argv[])
{
	char* iValue = NULL;
	char* oValue = NULL;

	bool IFlag = false;

	int c;

	// opterr = 0;

	while ((c = getopt(argc, argv, "hi:o:I")) != -1)
	{
		switch (c)
		{
			case 'h':
				//todo: display help
				return 0;;
			case 'i':
				iValue = optarg;
				break;
			case 'o':
				oValue = optarg;
				break;
			case 'I':
				IFlag = true;
				break;
			case '?':
				//todo
				cout << "?" << endl;
				return -1;
			default:
				//todo
				return -1;
		}
	}

	if (!iValue) //check for NULL iValue
	{
		cout << "ERROR: an input file is required" << endl;
		return -1;
	}

	if (IFlag) //check if an image is being used
	{

		//check that the input file extension is valid for images
		if (!isValidImageFileName(iValue)) 
		{
			cout << "ERROR: invalid file type input for an image." << endl;
			return -1;
		}

		//check if the name of an output file has been given
		if (oValue)
		{
			//check that the output file extension is valid for images
			if (!isValidImageFileName(iValue))
			{
				cout << "ERROR: invalid file type output for an image." << endl;
				return -1;
			}

			annotateImage(iValue, oValue);
		}
		else
		{
			annotateImage(iValue);
		}

	}
	else
	{

		//check that the input file extension is valid for videos
		if (!isValidVideoInputFileName(iValue)) {
			cout << "ERROR: invalid file type input for a video." << endl;
			return -1;
		}

		//check if the name of an output file has been given
		if (oValue)
		{
			//check that the output file extension is valid for videos
			if (!isValidVideoOutputFileName(oValue))
			{
				cout << "ERROR: invalid file type output for a video." << endl;
				return -1;
			}

			annotateVideo(iValue, oValue);
		}
		else
		{
			annotateVideo(iValue);
		}

	}

	return 0;

}
