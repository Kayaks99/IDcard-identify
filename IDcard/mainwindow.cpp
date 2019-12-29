#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->preImg->setStyleSheet("QLabel{border:2px solid rgb(0, 0, 0);}");
    ui->retImg->setStyleSheet("QLabel{border:2px solid rgb(0, 0, 0);}");
    ui->Head->setStyleSheet("QLabel{border:2px solid rgb(0, 0, 0);}");
    connect(ui->openButton,SIGNAL(clicked(bool)),this,SLOT(openButton_clicked()));
    connect(ui->runButton,SIGNAL(clicked(bool)),this,SLOT(runButton_clicked()));

    connect(ui->putText,SIGNAL(clicked(bool)),this,SLOT(putText()));
    connect(ui->putHead,SIGNAL(clicked(bool)),this,SLOT(putHead()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openButton_clicked()
{
    fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),
                                    ".",tr("Image Files (*.png *.jpg *.bmp)"));
    qDebug()<<"filenames:"<<fileName;
    if(!fileName.isEmpty())
    {
        img.load(fileName);//获得图像
        if(!img.load(fileName))
            qDebug("no!!");
        double bi = 500.0/img.width();
        QImage show = img.scaled(500,img.height()*bi);
        preimg = img.scaled(856, 540);
        h = img.height()*bi;
        //qDebug()<<img.height()*bi;
        ui->preImg->setAlignment(Qt::AlignCenter);
        //ui->preImg->setScaledContents(true);
        ui->preImg->setPixmap(QPixmap::fromImage(show));
    }
}

void MainWindow::runButton_clicked()
{
    QStringList strList = fileName.split("/");
    int last = strList.length();
    fileName=fileName.section('/',0,last-2);
    fileName=fileName+'/';
    //qDebug()<<fileName;
    QImage tmp = p.preTreat(preimg,fileName);
    //usePy();
    double bi = 500.0/tmp.width();
    QImage show = tmp.scaled(500,h);
    //qDebug()<<img.height()*bi;
    ui->retImg->setAlignment(Qt::AlignCenter);
    ui->retImg->setPixmap(QPixmap::fromImage(show));
}

void MainWindow::usePy()
{
    qDebug("py");
    Py_Initialize();
    if ( !Py_IsInitialized() )
    {
        return;
    }
    PyObject* pModule = PyImport_ImportModule("01");
    if (!pModule) {
            qDebug()<< "Cant open python file!!!!\n" << endl;
            return;
        }
   PyObject* pFunmain= PyObject_GetAttrString(pModule,"func");

    if(!pFunmain){
        qDebug()<<"Get function failed"<<endl;
        return;
    }
    // 创建元组设置参数
    const std::string s  = fileName.toStdString();
    const char *ss = s.c_str();
    //const char *ss = "C:/Users/Siyun/Desktop/IDcard/Debug/";
    PyObject* args = PyTuple_New(1);   // 2个参数
    PyObject* arg1 = Py_BuildValue("s", ss); // 参数一设为fileName
    PyTuple_SetItem(args, 0, arg1);

    PyObject_CallObject(pFunmain,args);
    Py_Finalize();
    qDebug()<<"pyend";
}

void MainWindow::usePy2()
{
    QString cmd;
    pyName = "C://Users//Siyun//Desktop//IDcard//Debug//debug//";
    const std::string s  = pyName.toStdString();
    const char *ss = s.c_str();
    cmd.sprintf("python %s01.py",ss);
    system(cmd.toStdString().data());
}



void MainWindow::putText()
{
    usePy();
    QString displayString;
    QFile file(fileName+"output.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    QByteArray byteArray = file.readAll();
    displayString = QString::fromLocal8Bit(byteArray.data());
    ui->textEdit->clear();
    ui->textEdit->setPlainText(displayString);
}

void MainWindow::putHead()
{
    QString headName = fileName+"head.jpg";
    img.load(headName);//获得图像
    if(!img.load(headName))
        qDebug("no!!");
    double bi = 160.0/img.width();
    QImage show = img.scaled(160,img.height()*bi);
    //qDebug()<<img.height()*bi;
    ui->Head->setAlignment(Qt::AlignCenter);
    //ui->preImg->setScaledContents(true);
    ui->Head->setPixmap(QPixmap::fromImage(show));
}

//void MainWindow::read_slot()
//{
//    qDebug()<<"***";
//    while(process->canReadLine())
//    {
//        char output[1024];
//        long long ret = process->readLine(output,1023);

//        QString tmp;
//        tmp = output;
//        qDebug()<<tmp;
//    }
//}
