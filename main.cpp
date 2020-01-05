

#include <string.h>
#include <unistd.h>

#include <iostream>

#include "laneLines.hpp"


using namespace std;


//todo: had to remove const from argv for this to compile with getopt, probably should make changes elsewhere
int main(int argc, char *argv[])
{

	char* vValue = NULL;
	char* iValue = NULL;
	char* oValue = NULL;
	int c;

	// opterr = 0;

	//todo: change this to be hi:o:I to match up with the original project
	while ((c = getopt(argc, argv, "v:i:o:")) != -1)
	{
		switch (c)
		{
			case 'v':
				vValue = optarg;
				break;
			case 'i':
				iValue = optarg;
				break;
			case 'o':
				oValue = optarg;
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

	if (!(vValue || iValue)) //both are null
	{
		cout << "ERROR: An argument for either -v or -i is required." << endl;
		return -1;
	}

	if (vValue && iValue) //both are not null
	{
		cout << "ERROR: Cannot pass in argument for both a video and an image." << endl;
		return -1;
	}

	if (vValue != NULL)
	{
		//todo: check that vValue is a supported opencv video type
		if (oValue == NULL)
		{
			annotateVideo(vValue);
		}
		else
		{
			//check that oValue is a .avi
			char* dot = strrchr(oValue, '.');
			if (dot && (strcmp(dot, ".avi") || strcmp(dot, ".AVI")))
			{
				annotateVideo(vValue, oValue);
			}
			else
			{
				cout << "ERROR: must use the .avi or .AVI extension for video output" << endl;
			}
			
		}
	}
	else
	{
		//todo: check that iVAlue is a supported opencv image type
		if (oValue == NULL)
		{
			annotateImage(iValue);
		}
		else
		{
			//todo: validate image output type
			annotateImage(iValue, oValue);
		}
	}

	return 0;
}
