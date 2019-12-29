#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <Python.h>
#include <QCoreApplication>
#include <math.h>
#include <QImage>
#include <QDebug>

class preprocess
{

public:
    QImage preTreat(QImage I,QString fileName);

private slots:



};

#endif // PREPROCESS_H
