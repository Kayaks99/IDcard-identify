#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <Python.h>
#include <QCoreApplication>
#include <math.h>
#include "mainwindow.h"
#include "preprocess.h"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;


int main(int argc, char ** argv){

    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    return a.exec();
}






