#include "utils.h"
#include "config.h"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/mat.hpp>
#include <alpr.h>

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
#include <sstream>
#include <cmath>

using namespace std;
using namespace cv;
using namespace alpr;
namespace fs = filesystem;

#define PI 3.14159265358979323846264338327950288419716939937510

/*======== SUBFUNCTIONS IMPLEMENTATION ==========*/

/* String and directory tools */
bool is_supported_file(fs::path path){
	if (!path.has_extension()){
		return false;
	}

	string ext = path.extension();

	if (ext == ".jpg")  return true;
	if (ext == ".JPG")  return true;
	if (ext == ".jpeg") return true;
	if (ext == ".JPEG") return true;
	if (ext == ".png")  return true;
	if (ext == ".PNG")  return true;
	if (ext == ".pdf")  return true;
	if (ext == ".PDF")  return true;
	if (ext == ".webp") return true;
	if (ext == ".WEBP") return true;
	if (ext == ".bmp")  return true;
	if (ext == ".BMP")  return true;
	if (ext == ".svg")  return true;
	if (ext == ".SVG")  return true;
	if (ext == ".heic") return true;
	if (ext == ".HEIC") return true;
	if (ext == ".raw")  return true;
	if (ext == ".RAW")  return true;
	if (ext == ".txt")  return true;
	if (ext == ".csv")  return true;

	DISPLAY_ERR(path << " format \"" << ext << "\" is not supported.");
    return false;
}

stack<string> *list_files(const char *path){
	stack<string> *file_paths = new stack<string>();
	fs::directory_entry f = fs::directory_entry(path);

	// File
	if (f.is_regular_file() && is_supported_file(f.path())){
		string ext = f.path().extension();
		if (ext == ".csv" || ext == ".txt"){
			// Text file containing images path
			ifstream infile(path);
			string line;
			while (getline(infile, line)){
				file_paths->push(line);
				DISPLAY("Added: " << line);	
			}
		} else {
			// Regular image file
			file_paths->push((string) f.path());
			DISPLAY("Added: " << f.path());
		}
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

bool copy_top(stack<string> *s1, stack<string> *s2){
	if (s1->empty()){
		DISPLAY_ERR("Empty stack.");
		return true;
	}
	s2->push(s1->top());

	return false;
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
		DISPLAY_ERR("Failed to write picture " << tmp);
	}
}

void save_picture(const Mat &picture, string path){
	bool success = imwrite(path, picture);
	if (success){
		DISPLAY("Wrote picture " << path);
	}
	else{
		DISPLAY_ERR("Failed to write picture " << path);
	}
}

void rename(string from, string to){
	if (fs::exists(fs::path(from)) && fs::exists(fs::path(to))){
        fs::rename(fs::path(from), fs::path(to));
		DISPLAY("Moved " << from << " to " << to);
	} else {
        DISPLAY_ERR(from << " and/or " << to << " do not exist");
	}
}

void remove(string path){
	if (fs::exists(fs::path(path))){
		if (fs::remove(fs::path(path))){
			DISPLAY("Removed " << path);
		} else {
			DISPLAY_ERR("Unable to remove " << path);
		}
	} else {
		DISPLAY_ERR(path << " does not exist");
	}
}

void remove_empty_directories(string path){ 
	if (fs::is_directory(fs::path(path))){
		while (fs::is_empty(path)){
			//if (!fs::remove(fs::path(path))){
			DISPLAY("DEBUG: " << path)
			if (false){
				DISPLAY_ERR("Unable to remove empty directory " << path);
				break;
			}
			if (fs::path(path).has_parent_path()){
				path = fs::path(path).parent_path().string();
			} else { break;}
		}
	} else {
		DISPLAY_ERR(path << " is not empty");
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

bool build_dir(const char *path){
	struct stat info;
	if(stat(path, &info) != 0){
		if (mkdir(path, S_IRWXU) != 0){
			DISPLAY_ERR("Couldn't create " << path);
			return false;
		}
	}
	else if (!S_ISDIR(info.st_mode)){ 
		DISPLAY_ERR(path << " is not a directory");
		return false;
	}
	return true;
}

string get_filename(const string &filepath){
	return fs::path(filepath).stem();
}

string get_file_extension(const string &filepath){
    return fs::path(filepath).extension();
}

bool replace(string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

string add_addon(string path, string addon){
	fs::path tmp = fs::path(path);
	tmp.replace_extension(addon+(string)tmp.extension());
	return (string) tmp;
}

string select_output_dir(const string out_root, const string in_root,
						 const string filepath, const bool respect_input_path){
	if (respect_input_path){
		// Getting absolute path for safer replacement
		string abs_out_root  = (string) fs::absolute(fs::path(out_root)) + (string) "/";
		string abs_in_root   = (string) fs::absolute(fs::path(in_root)) + (string) "/";
		string abs_filepath  = (string) fs::absolute(fs::path(filepath));

		// Normalizing every "//" and "/./" that causes replacing error
		while (replace(abs_out_root, "//", "/")){}
		while (replace(abs_out_root, "/./", "/")){}
		while (replace(abs_in_root, "//", "/")){}
		while (replace(abs_in_root, "/./", "/")){}
		while (replace(abs_filepath, "//", "/")){}
		while (replace(abs_filepath, "/./", "/")){}

		// Trying to replace input path with output path
		if (replace(abs_filepath, abs_in_root, abs_out_root)){
			return (string) fs::path(abs_filepath).replace_extension("");
		}

		// If couldn't replace, last return will be called
		DISPLAY_ERR("Couldn't respect original path of " << filepath
		<< ".\n\tWriting into " << get_filename(filepath));
	}

	return out_root+(string) "/"+get_filename(filepath);
}

bool build_directories(const string path){
	// Choosing wether or not the last part has to be built
	string tmp = path;
	if (fs::path(path).has_extension()){
		tmp = (string) fs::path(path).parent_path();
	}

	// Building directories
	fs::create_directories(tmp);
	if (!fs::directory_entry(tmp).exists()){
		DISPLAY_ERR("Failed to create directories " << tmp);
		return false;
	}
	return true;
}

//Used for blur-only option to allow any order for the 4 points
void sort_corners(vector<Point> &corners){ //FIXME: Sorting the 4 points in not working
	return;
	if (corners.size() != 4){
		cerr << "\033[1;31mERROR:\033[0m Area is not having exactly 4 points." << endl;
		exit(EXIT_FAILURE);
	}

	// Computing center
	vector<double> angles;
	Point center = {0, 0};
	for(auto&& pt: corners){
		center.x += pt.x;
		center.y += pt.y;
	}
	center.x /= 4;
	center.y /= 4;

	// Computing angle from (1, 0) (work.. maybe ??)
	double ang;
	Point vect;
	for(auto&& pt: corners){
		vect.x = pt.x - center.x;
		vect.y = pt.y - center.y;
		ang = acos((double) vect.x/sqrt(vect.x*vect.x + vect.y*vect.y));
		//ang = vect.y < 0 ? ang + PI : ang;
		//cout << ang << endl;
		angles.push_back(ang);
	}

	// Sorting according to angle (works)
	int i;
	bool sorted = false;
	while(!sorted){
		sorted = true;
		for(i = 0; i < 3; i++){
			if (angles[i] > angles[i+1]){
				// Swaping angles
				ang = angles[i];
				angles[i] = angles[i+1];
				angles[i+1] = ang;

				// Swapping points
				vect = corners[i];
				corners[i] = corners[i+1];
				corners[i+1] = vect;
				
				sorted = false;
			}
		}
	}
	
	// Putting the top-left corner first
	//TODO
	//cout << endl;
}

vector<vector<Point> > parse_location(const string str_location){
	vector<vector<Point> > buffer = vector<vector<Point> >();
	vector<Point> tmp_corner = vector<Point>();
	Point tmp_point = Point();
	 
	// PARSING
	size_t us_loc_0 = 0;
	size_t us_loc_1 = 0;
	int val;
	int i = 0;
	while(us_loc_1 < str_location.length()){
		us_loc_0 = i == 0 ? us_loc_1 : us_loc_1 + 1;
		us_loc_1 = str_location.find('_', us_loc_0);
		val = stoi(str_location.substr(us_loc_0, us_loc_1 - us_loc_0));
		if (i&1){
			tmp_point.y = val;
			tmp_corner.push_back(tmp_point);
		} else {
			tmp_point.x = val;
		}
		i++;

		// Adding the 4 corners into the buffer
		if (i % 8 == 0){
			if (tmp_corner.size() != 4){
				cerr << "\033[1;31mERROR:\033[0m Area is not having exactly 4 points." << endl;
				exit(EXIT_FAILURE);
			}
			buffer.push_back(tmp_corner);
			tmp_corner = vector<Point>();
		}
	}

	// VERIFICATION
	if (i % 8 != 0 || buffer.size() != (size_t) i/8){
		cerr << "\033[1;31mERROR:\033[0m The blur location must contain a multiple of 8 values." << endl;
		exit(EXIT_FAILURE);
	}
	return buffer;
}

void plate_corners(const vector<AlprPlateResult> &results,
				 vector<vector<Point> > &corners,
				 vector<string> &numbers){
	size_t i, j;
	vector<Point> tmp_vect;
	Point tmp_pt;
	for (i = 0; i < results.size(); i++){
		for (j = 0; j < 4; j++){
			tmp_pt.x = results[i].plate_points[j].x;
			tmp_pt.y = results[i].plate_points[j].y;
			tmp_vect.push_back(tmp_pt);
		}
		corners.push_back(tmp_vect); 
		numbers.push_back(results[i].bestPlate.characters);
		tmp_vect.clear();
	}
}
