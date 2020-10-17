#include "mainwindow.h"
#include "processConfig.h"

#include <QApplication>

#include <fstream>

using namespace std;

bool verbose; /// Whether or not information should be displayed
bool save_log; /// Whether or not logs should be saved (default: false)
ofstream log_ostream; /// Stream to the file where the logs will be saved

void usage(char* name){
	cout <<"\
Usage: " << name << " -i <path to picture or directory>\
-o <output directory>\n\
Type -h or --help for more details.\n";
}

int main(int argc, char *argv[])
{
    // Verifying input
    if (argc == 1){
		usage(argv[0]);
		return -1;
	}

    // Global variable initialization
	save_log = false;
	verbose  = false;

    QApplication a(argc, argv);
    MainWindow w;
    w.initProcess(argv);
    w.show();
    a.exec();
    w.finalizeProcess();
    return EXIT_SUCCESS;
}
