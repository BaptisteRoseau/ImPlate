#include "utils.h"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/mat.hpp>
//#include <opencv4/opencv2/highgui/highgui.hpp>
//#include <opencv4/opencv2/imgproc/imgproc.hpp>

#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <cstring>
#include <random>
#include <stack>
#include <fstream>

using namespace std;
using namespace cv;
namespace fs = filesystem;

/*======== SUBFUNCTIONS IMPLEMENTATION ==========*/

/* String and directory tools */
bool is_supported_file(fs::path path){
	if (!path.has_extension()){
		return false;
	}

	string ext = path.extension();
    //TODO: Find supported extensions and add them here, then remove commentary
    return true;
}

stack<string> *list_files(const char *path){
	stack<string> *file_paths = new stack<string>();
	fs::directory_entry f = fs::directory_entry(path);

	// File
	if (f.is_regular_file()){// && is_supported_file(f.path())){
		file_paths->push((string) f.path());
		DISPLAY("Added: " << f.path());
		return file_paths;
	}

	// Directory
	if (f.is_directory()){
		for(auto& p: fs::recursive_directory_iterator(path)){
			if (p.is_regular_file() && is_supported_file(p.path())){
                file_paths->push((string) p.path());
                DISPLAY("Added: " << p.path());
			}
		}
		return file_paths;
	}

	// ERROR case
	DISPLAY_ERR(path << " is not a file nor a directory.");
	return NULL;
}

Mat open_picture(const string path){
	Mat ret = imread(path);
    if (ret.empty()){
        DISPLAY_ERR("Failed to open " << path);
    } else {
        DISPLAY("Opened " << path);
    }
    return ret;
}

string stack_next(stack<string> *s){
    if (s->empty()){
        DISPLAY_ERR("Empty file stack.");
        return NULL;
    }
    string tmp = s->top();
    s->pop();
    return tmp;
}

void save_picture(const Mat &picture, string dir, string name){
	string tmp = dir+'/'+name;
	bool success = imwrite(tmp, picture);
	if (success){
		DISPLAY("Wrote picture " << tmp);
	}
	else{
		DISPLAY("Failed to write picture " << tmp);
	}
}

string str_normalize(string &s){
	for (size_t i = 0; i < s.length(); i++){
		switch (s[i]){
			case ' ': s[i] = '_'; break;
			case '(': s[i] = '_'; break;
			case ')': s[i] = '_'; break;
			case '[': s[i] = '_'; break;
			case ']': s[i] = '_'; break;
		}
	}
    return s;
}

int build_dir(const char *path){
	struct stat info;
	if(stat(path, &info) != 0){
		if (mkdir(path, S_IRWXU) != 0){
			DISPLAY_ERR("Couldn't create " << path);
			return EXIT_FAILURE;
		}
	}
	else if (!S_ISDIR(info.st_mode)){ 
		DISPLAY_ERR(path << " is not a directory");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

string get_filename(const string &filepath){
	return fs::path(filepath).stem();
}

string get_file_extension(const string &filepath){
    return fs::path(filepath).extension();
}

//TODO: Implement this
string select_output_dir(const string out_dir, const string in_path, const string filepath, const bool respect_input_path){
	(void) in_path;
	string root = out_dir+(string) "/"; 
	// Respect the original path into input directory if necessary
	if (respect_input_path){
		fs::path path = fs::path(filepath).relative_path();
		cout << path << endl;
		string buffer = root;
		int i = 0;
		for (const auto& part : path){
			if (i != 0){
				buffer += (string) part.stem()+"/";
			}
			i++;
		}
		cout << buffer << endl;
		return root+get_filename(filepath);
	}

	// Else, return only the filename
	return root+get_filename(filepath);
}

void sort_corners(vector<Point> &corners){

}

vector<Point> parse_location(const string str_location){
	/* CHANGE L'INPUT DES OPTIONS AUSSI */
	vector<Point> buffer = vector<Point>();
	Point tmp_pt = Point();
	 
	// PARSING
	size_t us_loc_0 = 0;
	size_t us_loc_1 = 0;
	int val;
	int i = 0;
	while(us_loc_1 < str_location.length()){
		us_loc_0 = us_loc_1;
		us_loc_1 = str_location.find('_', us_loc_0);
		val = stoi(str_location.substr(us_loc_0, us_loc_1));
		if (i&1){
			tmp_pt.y = val;
			buffer.push_back(tmp_pt);
		} else {
			tmp_pt.x = val;
		}
		i++;
		DISPLAY(stoi(str_location.substr(us_loc_0, us_loc_1)));
	}
	return buffer;
}
