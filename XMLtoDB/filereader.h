#ifndef FILEREADER_H
#define FILEREADER_H

#include<sqlqueryprocessor.h>
#include<QFile>
#include<QDebug>
#include<QSqlQuery>
#include<QXmlStreamReader>
#include<QObject>
#include<QSqlDatabase>
#include<QVector>


class FileReader : public QObject
{
    Q_OBJECT


protected:
    QFile *CurrentFile;
    short FileType;
    QString Version;
    QXmlStreamReader *Parser;
    QSqlQuery *PersonalInfoQuery, *DocumentsQuery, *ClosedCasesQuery, *ServicesQuery, *AppointmentsQuery, *SanctionsQuery;
    QSqlQuery *CasesQuery , *BillQuery, *HiTechCasesQuery;
    QStringList Package;
    QVector <int> FileTypes;
    QVector<QSqlQuery> QueryQueue;
    QString PackageNumber;
    QString LFilePath, LFileName;
    SQLQueryProcessor *QueryProcessor;
   QSqlDatabase db,regdb;


public:
    FileReader();
    ~FileReader();
    bool ReadFile(QString , QString);
    bool Check(QString);
    void ParseFile();
    QString GetFileName();
    void SetFilePath(QString);
    QString GetLFileName();
    QString GetLFilePath();
    QString GetFileFromPackage(int index);
    QStringList GetPackage();
    void addFileToPackage(QString);
    void SetCurrentFile(QString);
    void PersonalInfoRequest();


};

#endif // FILEREADER_H
