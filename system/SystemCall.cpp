#include "SystemCall.h"

void SystemCall::wait_for_user_keypress()
{
	//note: it is generally considered bad practice to use this type of control, 
	//but it is useful while developing, so that results can be viewed immediately 
	//and easily, even without using watches and other debugger tools.

	#ifdef EnableUserPause
	
	cin.clear();
	std::cout << "Press Enter to continue...";
	cin.ignore();
	
	#endif
}

#ifdef Windows

vector<string> SystemCall::get_all_files_names_within_folder(string folder)
{
	vector<string> names;
	string search_path = folder + "/*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

#endif

#ifdef Linux
vector<string> SystemCall::get_all_files_names_within_folder(string folder)
{
	vector<string> names;


	DIR *dp;
	struct dirent *dirp;

	if((dp  = opendir(folder.c_str())) == NULL) {
		cout << "Error(" << errno << ") opening " << folder << std::endl;
		throw runtime_error("directory not found");
	}

	while ((dirp = readdir(dp)) != NULL) {
        string name = string(dirp->d_name);
        if (name != "." && name != "..")        //match windows functionality
            names.push_back(name);
	}
	closedir(dp);

	return names;
}

#endif
