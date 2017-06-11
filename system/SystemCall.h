#pragma once

#include "applicationDefines.h"
#include <stdexcept>


#ifdef Windows
#include <Windows.h>
#endif

#ifdef Linux
#include<dirent.h>
#include <sys/types.h>
#endif


#include <vector>
#include <string>
#include <iostream>





using namespace std;
class SystemCall
{
public:
	static vector<string> get_all_files_names_within_folder(string folder);

	static void wait_for_user_keypress();
};



