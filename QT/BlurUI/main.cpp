#include "mainwindow.h"
#include "processConfig.h"

#include <QApplication>

#include <fstream>

using namespace std;

bool verbose; /// Whether or not information should be displayed
bool save_log; /// Whether or not logs should be saved (default: false)
ofstream log_ostream; /// Stream to the file where the logs will be saved

int main(int argc, char *argv[])
{
    /* if (argc == 1){
		usage(argv[0]);
		return -1;
	} */

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
