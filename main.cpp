#include "dataTypes\RgbHistogram.h"
#include "testing.h"
#include <stdexcept>
#include <iostream>
#include "applicationDefines.h"
#include "SystemCall.h"
#include "ProgramManager.h"

using namespace std;

void showHelp()
{
	cout << "You must pass an argument to the program." << std::endl;
	cout << "Available arguments are: -tcd -si" << std::endl;
}

int main(int argc, char *argv[])
{

	if (argc < 3)
	{
		showHelp();
		SystemCall::wait_for_user_keypress();
		return 0;
	}

	string arg1 = string(argv[1]);
	string arg2 = string(argv[2]);
	try
	{
		if (arg1 == "-save")
		{
			cout << "Entered save image mode." << std::endl;
			cout << "(Not yet implemented)" << std::endl;
		}
		else if (arg1 == "-tcd" && arg2 == "-si")
		{
			ProgramManager::trainClassifyDisplaySingleImage();
		}
		else if (arg1 == "-tcd" && arg2 == "-cam")
		{
			ProgramManager::trainClassifyDisplayCameraStream();
		}
		else
		{
			showHelp();
		}
	}
	catch (const exception &e)
	{
		cerr << e.what() << std::endl;
	}

	SystemCall::wait_for_user_keypress();

	return 0;
}
