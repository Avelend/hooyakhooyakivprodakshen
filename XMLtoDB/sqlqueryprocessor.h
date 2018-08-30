#ifndef SQLQUERYPROCESSOR_H
#define SQLQUERYPROCESSOR_H

#include<QSqlQuery>
#include<QSqlDriver>
#include<QSqlDatabase>
#include<QXmlStreamReader>
#include<QFile>
#include<QObject>


class SQLQueryProcessor : public QObject
{
    Q_OBJECT
protected:
    QString LFilePackageNumber, LFileName;
    QString Version;
    QXmlStreamReader *PIParser;
    QFile *LFile;
    QSqlQuery *Q, *Q2, *RegDBQuery;
public:
    SQLQueryProcessor();
    SQLQueryProcessor(QString FileN, QString FileP, QSqlDatabase, QSqlDatabase RegDB);
    ~ SQLQueryProcessor();
    void PersInformationParser(QString);
    void CommitQueries(QSqlQuery *, QSqlQuery *, QSqlQuery *);

};

#endif // SQLQUERYPROCESSOR_H
