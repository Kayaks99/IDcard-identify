#include "preprocess.h"

using namespace cv;
using namespace std;
Mat src, dst, gray_src;
char input_image[] = "input image";
//char output_image[] = "output image";
Mat navieRemoveNoise(Mat mat, int pNum);
double  getAngle(double px1, double py1, double px2, double py2);
Mat preprocess2(Mat gray);
Mat preprocess3(Mat gray);
vector<Rect> findTextRegion(Mat img);
vector<Rect> findHeadRegion(Mat img);
void usePy();
static double GetMatAverage(const cv::Mat& src)
{
    CV_Assert(src.type() == CV_8UC1);
    double sum = 0.0;
    for (int y = 0; y < src.rows; ++y)
    {
        for (int x = 0; x < src.cols; ++x)
        {
            int value = src.at<uchar>(y, x);
            sum += value;
        }
    }

    return sum / (src.rows * src.cols);
}

/* @brief 计算单通道灰度图像的标准差

@param src 单通道灰度图
*/
static double GetMatStdDev(const cv::Mat& src, double meanValue)
{
    CV_Assert(src.type() == CV_8UC1);
    double sum = 0.0;
    for (int y = 0; y < src.rows; ++y)
    {
        for (int x = 0; x < src.cols; ++x)
        {
            int value = src.at<uchar>(y, x);
            double var = (value - meanValue)*(value - meanValue);
            sum += var;
        }
    }

    double stdDev = std::sqrt(double(sum) / double(src.rows * src.cols));
    return stdDev;
}
Mat QImageToMat(QImage image)
{
    Mat mat;
    switch (image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

QImage MatToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        //qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        //qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

QImage preprocess::preTreat(QImage I, QString fileName)
{
    src = QImageToMat(I);
    resize(src, src, Size(856, 540));
    Mat ret = src;
    //namedWindow(input_image,  WINDOW_AUTOSIZE);
    //namedWindow(output_image,  WINDOW_AUTOSIZE);
    //imshow(input_image, src);

    //灰度化
    Mat binary;
    cvtColor(src, binary, COLOR_BGR2GRAY, 1);
    //imshow("gray", binary);

    //二值化
    adaptiveThreshold(binary, binary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 25, 10);
    //imshow("2", binary);
    navieRemoveNoise(binary, 3);
    ////腐蚀
    //Mat element2 = getStructuringElement(MORPH_RECT, Size(2, 2));      //腐蚀        Imgproc.erode(mat, erode, element, new Point(-1, -1), 1);
    //erode(binary, binary, element2, Point(-1, -1), 1);
    //imshow("erode", binary);

    ////边缘检测
    GaussianBlur(binary, binary, Size(3, 3), 0);
    //imshow("gau", binary);
    //Canny(binary, binary, 50, 150);
    //imshow("edge", binary);
    //
    ////	降噪
    navieRemoveNoise(binary, 1);
    //imshow("redu", binary);
    ////	膨胀
    //blur(binary, binary, Size(5, 5));
    //imshow("blur", binary);


    //Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    //dilate(binary, binary, element1,Point(-1, -1), 1);
    //imshow("dilate", binary);

    //


    // 轮廓发现
    //bitwise_not(binary, dst, Mat());
    vector<vector<Point>> contours;
    vector<Vec4i> hireachy;
    int width = src.cols;
    int height = src.rows;
    Mat drawImage = Mat::zeros(src.size(), CV_8UC3);
    findContours(binary, contours, hireachy,  RETR_LIST, CHAIN_APPROX_SIMPLE, Point());

    for (size_t t = 0; t < contours.size(); t++) {
        Rect rect = boundingRect(contours[t]);//轮廓最小外接矩形
        float area = rect.area();
        //printf("rect.width : %d, src.cols %d \n ", rect.width, src.cols);
        if (area>200)
        {
            drawContours(drawImage, contours, static_cast<int>(t), Scalar(0, 0, 255), 2, 8, hireachy, 0, Point());
        }
        //rectangle(ret,rect,cv::Scalar(0,255,0),2,LINE_8);
    }
    //imwrite("contours.jpg", drawImage);


    // 绘制直线
    vector<Vec4i> lines;
    vector<Vec4i> lines0;

    Mat contoursImg;
    int accu = min(width * 0.5, height *0.5);
    cvtColor(drawImage, contoursImg, COLOR_BGR2GRAY);
    Mat linesImage = Mat::zeros(src.size(), CV_8UC3);
    HoughLinesP(contoursImg, lines, 1, CV_PI / 180.0, 10, 0.5*accu, 10);//zheli!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    Vec4i maxLine;
    maxLine[0] = maxLine[1] = maxLine[2] = maxLine[3] = 0;
    for (int x = 0; x < lines.size(); x++)
    {
        Vec4i vec = lines[x];
        double x1 = vec[0], y1 = vec[1], x2 = vec[2], y2 = vec[3];
        double newLength = sqrt(abs((x1 - x2)* (x1 - x2) + (y1 - y2)* (y1 - y2)));
        double oldLength = sqrt(abs((maxLine[0] - maxLine[2])* (maxLine[0] - maxLine[2]) + (maxLine[1] - maxLine[3])* (maxLine[1] - maxLine[3])));
        if (newLength > oldLength)
        {
            maxLine = vec;
        }
    }

    double angle = getAngle(maxLine[0], maxLine[1], maxLine[2], maxLine[3]);
    //Rotate(binary, angle);
    //if (angle > 45)angle = 90 - angle;
    std::cout << angle << endl;
        // 计算旋转后输出图形的尺寸
        int rotated_width = ceil(binary.rows * fabs(sin(angle * CV_PI / 180)) + binary.cols * fabs(cos(angle * CV_PI / 180)));
        int rotated_height = ceil(binary.cols * fabs(sin(angle * CV_PI / 180)) + binary.rows * fabs(cos(angle * CV_PI / 180)));

        // 计算仿射变换矩阵
        Point2f center(binary.cols / 2, binary.rows / 2);
        Mat rotate_matrix = getRotationMatrix2D(center, angle, 1.0);

        // 防止切边，对平移矩阵B进行修改
        rotate_matrix.at<double>(0, 2) += (rotated_width - binary.cols) / 2;
        rotate_matrix.at<double>(1, 2) += (rotated_height - binary.rows) / 2;

        // 应用仿射变换
        warpAffine(src, src, rotate_matrix, Size(rotated_width, rotated_height), INTER_LINEAR, 0, Scalar(255, 255, 255));
        //imshow("result", src);

        //HoughLinesP(binary, lines, 1, CV_PI / 180, 10, 10, 10);
        Mat dstt;
        //cvtColor(src, dstt, COLOR_BGR2GRAY, 1);
        ////imshow("gray", binary);

        ////二值化
        //adaptiveThreshold(dstt, dstt, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 25, 10);
        //navieRemoveNoise(dstt, 1);
        ////imshow("2", binary);
        //imshow("dst", dstt);


        //1.转化成灰度图
        //Mat gray;
        cvtColor(src, dstt, COLOR_BGR2GRAY,1);

        //2.形态学变换的预处理，得到可以查找矩形的轮廓
        Mat dimg;
        cvtColor(drawImage, dimg, COLOR_BGR2GRAY);
        Mat dilation = preprocess2(dimg);

        //3.查找和筛选文字区域
        vector<Rect> rects = findTextRegion(dilation);

        //3.2查找和筛选头像区域
        Mat element = getStructuringElement(MORPH_RECT, Size(5, 30));
        Mat element1 = getStructuringElement(MORPH_RECT, Size(35, 5));
        Mat dil;
        dilate(dstt,dil,element);
        Mat dil2;
        dil = threshold(dil,dil2,170,255,THRESH_BINARY_INV );
        vector<Rect> rects2 = findHeadRegion(dil2);

        //4.保存轮廓
        //for each (RotatedRect rect in rects)
        string sout;
        int kk=1;
        for (int i = 0; i < rects.size(); i++)
        {
            Rect rect = rects.at(i);
            if (rect.tl().x > 450 || rect.height*2 > rect.width) {
                //imwrite("head.jpg", dstt(rect));
                continue;
            }
            rectangle(ret,rect,cv::Scalar(0,255,0),2,LINE_8);
            sout = fileName.toStdString() + to_string(kk) + ".jpg";//"(" + fileName.toStdString() + ")" +
            qDebug()<<QString::fromStdString(sout);
            kk++;
            imwrite(sout, dstt(rect));

        }

        //4.2保存轮廓
        for (int i = 0; i < rects2.size(); i++)
        {
            Rect rect = rects2.at(i);

            if (rect.tl().x < 400) {
                continue;
            }
            imwrite(fileName.toStdString() + "head.jpg", dstt(rect));
            rectangle(ret,rect,cv::Scalar(255,255,0),2,LINE_8);

        }
        //5.显示带轮廓的图像

        QImage rr = MatToQImage(ret);
        return rr;
}

Mat navieRemoveNoise(Mat mat, int pNum)
{
    int i, j, m, n, nValue, nCount;
    int nWidth = mat.cols;
    int nHeight = mat.rows;
    // 如果一个点的周围都是白色的，而它确是黑色的，删除它
    for (j = 1; j < nHeight - 1; ++j)
    {
        uchar* data = mat.ptr<uchar>(j);
        for (i = 1; i < nWidth - 1; ++i)
        {
            nValue = data[i];
            if (nValue == 0)
            {
                nCount = 0;
                // 比较以(j ,i)为中心的9宫格，如果周围都是白色的，同化
                for (m = j - 1; m <= j + 1; ++m)
                {
                    for (n = i - 1; n <= i + 1; ++n)
                    {
                        uchar* data0 = mat.ptr<uchar>(m);
                        int tmp = data0[n];
                        if (tmp == 0)
                        {
                            nCount++;
                        }
                    }
                }
                if (nCount <= pNum)
                {
                    // 周围黑色点的个数小于阀值pNum,把该点设置白色
                    data[i] = 255;
                }
            }
            else
            {
                nCount = 0;
                // 比较以(j ,i)为中心的9宫格，如果周围都是黑色的，同化
                for (m = j - 1; m <= j + 1; ++m)
                {
                    for (n = i - 1; n <= i + 1; ++n)
                    {
                        uchar* data0 = mat.ptr<uchar>(m);
                        int tmp = data0[n];
                        if (tmp == 0)
                        {
                            nCount++;
                        }
                    }
                }
                if (nCount >= 7)
                {
                    // 周围黑色点的个数大于等于7,把该点设置黑色;即周围都是黑色
                    data[i] = 0;
                }
            }
        }
    }
    return mat;
}

double  getAngle(double px1, double py1, double px2, double py2)
{        //两点的x、y值
    double x = px2-px1;
    double y = py2-py1;
    double hypotenuse = sqrt(pow(x, 2)+pow(y, 2));
    //斜边长度
    double cos = x/hypotenuse;
    if (cos < 0)cos = -cos;
    double radian = acos(cos);        //求出弧度
    double angle = 180/(CV_PI/radian);
    ////用弧度算出角度
    //if (y<0)
    //{
    //	angle = -angle;
    //} else if ((y == 0) && (x<0))
    //{
    //	angle = 180;
    //}

    //while (angle<0){
    //	angle = angle +90;
    //}
    if (y / x > 0)angle = 180 - angle;
    else ;
    return -angle;
}


Mat preprocess2(Mat gray)
{
    //1.Sobel算子，x方向求梯度
    Mat sobel;
    Sobel(gray, sobel, CV_8U, 1, 0, 3);

    //2.二值化
    Mat binary;
    threshold(sobel, binary, 0, 255, THRESH_OTSU + THRESH_BINARY);

    //3.膨胀和腐蚀操作核设定
    Mat element1 = getStructuringElement(MORPH_RECT, Size(15, 4));
    //控制高度设置可以控制上下行的膨胀程度，例如3比4的区分能力更强,但也会造成漏检
    Mat element2 = getStructuringElement(MORPH_RECT, Size(30, 9));

    //4.膨胀一次，让轮廓突出
    Mat dilate1;
    dilate(binary, dilate1, element2);

    //5.腐蚀一次，去掉细节，表格线等。这里去掉的是竖直的线
    Mat erode1;
    erode(dilate1, erode1, element1);

    //6.再次膨胀，让轮廓明显一些
    Mat dilate2;
    dilate(erode1, dilate2, element2);

    //7.存储中间图片
//    imwrite("binary.jpg", binary);
//    imwrite("dilate1.jpg", dilate1);
//    imwrite("erode1.jpg", erode1);
//    imwrite("dilate2.jpg", dilate2);

    return dilate2;
}

vector<Rect> findTextRegion(Mat img)
{
    vector<Rect> rects;
    //1.查找轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(img, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));

    //2.筛选那些面积小的
    for (int i = 0; i < contours.size(); i++)
    {
        //计算当前轮廓的面积
        double area = contourArea(contours[i]);

        //面积小于1000的全部筛选掉
        if (area < 500)
            continue;

        double epsilon = 0.001*arcLength(contours[i], true);
        Mat approx;
        approxPolyDP(contours[i], approx, epsilon, true);

        //找到矩形，该矩形无方向
        Rect rect = boundingRect(contours[i]);

        //计算高和宽
        int m_width = rect.width;
        int m_height = rect.height;

        //筛选那些太细的矩形，留下扁的
        if (m_height > m_width * 2)
            continue;

        if (m_height < 25)
            continue;
        if (m_height > 50 && rect.tl().x < 80)
            continue;

        //符合条件的rect添加到rects集合中
        rects.push_back(rect);

    }
    return rects;
}

vector<Rect> findHeadRegion(Mat img)
{
    vector<Rect> rects;
    //1.查找轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(img, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));

    //2.筛选那些面积小的
    for (int i = 0; i < contours.size(); i++)
    {
        //计算当前轮廓的面积
        double area = contourArea(contours[i]);

        //面积小于1000的全部筛选掉
        if (area < 10000)
            continue;

        double epsilon = 0.001*arcLength(contours[i], true);
        Mat approx;
        approxPolyDP(contours[i], approx, epsilon, true);

        //找到矩形，该矩形无方向
        Rect rect = boundingRect(contours[i]);

        //计算高和宽
        int m_width = rect.width;
        int m_height = rect.height;

        //筛选那些太细的矩形，留下扁的
//        if (m_height < m_width)
//            continue;

        //符合条件的rect添加到rects集合中
        rects.push_back(rect);

    }
    return rects;
}
