#include <QCoreApplication>
#include<QDebug>
#include<QXmlStreamReader>
#include<filereader.h>
#include<QStringList>
#include<QDir>
#include<sqlqueryprocessor.h>
#include<QObject>
#include<QVector>
#include<QTime>
#include<QFileDialog>
#include<QApplication>
#include<QUrl>


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    QFileDialog fdialog;
    fdialog.setFileMode(QFileDialog::DirectoryOnly);
    fdialog.setDirectory("C:\\");
    fdialog.exec();
    QDir Directory = fdialog.directory();

    FileReader FileRr;
    QTime timer;

    QStringList FileList = Directory.entryList(QDir::Files);

    timer.start();
    foreach (QString File, FileList)
    {
        if(FileRr.ReadFile(Directory.path() , File)) FileRr.addFileToPackage(Directory.path() +"/"+ File);
    }

   FileRr.PersonalInfoRequest();

    for(int i = 0; i < FileRr.GetPackage().length(); ++i)
    {
        qDebug()<<"Start reading :"<<FileRr.GetFileFromPackage(i);
        FileRr.SetCurrentFile(FileRr.GetFileFromPackage(i));
        FileRr.ParseFile();
    }
            FileRr.FileReader::~FileReader();
            int T = timer.elapsed();

    qDebug()<<"Done in: "<<T/60000<<" minutes"<<(T%60000)/1000<<" seconds";

    return a.exec();
}
