#include "sqlqueryprocessor.h"
#include<qdebug.h>
#include<QVariant>
#include<QTime>
#include<QtConcurrent>


SQLQueryProcessor::SQLQueryProcessor(QString FileN , QString FileP, QSqlDatabase DB, QSqlDatabase RegDB)
{

    Q = new QSqlQuery(DB);
    Q2 = new QSqlQuery(DB);  
    RegDBQuery = new QSqlQuery(RegDB);

    LFileName = FileN;
    LFile = new QFile(FileP + "\\" + FileN);
    LFile->open(QIODevice::ReadOnly | QIODevice::Text);

}

void SQLQueryProcessor::PersInformationParser( QString PackageNumber)
{


    Q->prepare("INSERT INTO "
                             " personal_info (pacient_id, last_name, name, second_name, birth_date, gender, p_last_name, p_name, p_second_name, p_gender, birth_place, reg_id)"
                             " VALUES (:pacient_id, :last_name, :name, :second_name, :birth_date, :gender, :p_last_name, :p_name, :p_second_name, :p_gender, :birth_place, :reg_id)");

    Q2->prepare("INSERT INTO documents (pacient_id, document_type, document_series, document_serial_number, "
                            "snils, okato_g, okato_p, okato_st, v_polis, polis_series, polis_serial_number, smo, smo_ogrn, smo_ok, smo_nam, inv , new_born, new_born_w) "
                "VALUES (:pacient_id, :document_type, :document_series, :document_serial_number, :snils, :okato_g, :okato_p, 0 ,0 , 0 ,0 ,0 ,0 ,0 ,0, 0, 0, 0)");

    RegDBQuery->prepare("SELECT id FROM \"123\".people WHERE fam = :fam AND im = :im AND ot = :ot AND dr = :dr AND (ss = :ss OR (docs = :docs AND docn = :docn))");

    QTime Time;
    Time.start();
    QString Temp;
    if(!LFileName.isEmpty())
    {
    Temp = LFileName.split("_")[1];
    Temp = Temp.split(".")[0];
    }
    else { qDebug()<<"\"L\" File not found"; return;}
    if(PackageNumber == Temp  && LFile->isOpen())
    {
        LFile->reset();

        PIParser = new QXmlStreamReader(LFile);
        qDebug()<<"Reading personal info";

        while (!PIParser->atEnd() && !PIParser->hasError())
           {
               QXmlStreamReader::TokenType token = PIParser->readNext();
               if (token == QXmlStreamReader::StartDocument)
                   continue;

              if (token == QXmlStreamReader::StartElement && PIParser->name()=="PERS_LIST")
               {
                   while(PIParser->readNextStartElement() && (!PIParser->isEndElement() && !(PIParser->name()=="PERS_LIST")))
                   {
                       if(token == QXmlStreamReader::StartElement && PIParser->name()=="ZGLV")
                       {
                           while(PIParser->readNextStartElement() && (!PIParser->isEndElement() && !(PIParser->name()=="ZGLV")))
                           {
                               if (token == QXmlStreamReader::StartElement && PIParser->name()=="VERSION") Version = PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                               if (token == QXmlStreamReader::StartElement && PIParser->name()=="DATA") PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                               if (token == QXmlStreamReader::StartElement && PIParser->name()=="FILENAME") PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                               if (token == QXmlStreamReader::StartElement && PIParser->name()=="FILENAME1") PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                           }
                       }

                       if(token == QXmlStreamReader::StartElement && PIParser->name()=="PERS")
                       {

                                while(PIParser->readNextStartElement() && (!PIParser->isEndElement() && !(PIParser->name()=="PERS")))
                                {

                                            if (token == QXmlStreamReader::StartElement && PIParser->name()=="ID_PAC")
                                            {
                                                QString str =PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                Q->bindValue(":pacient_id" ,str );
                                                Q2->bindValue(":pacient_id" ,str );
                                            }
                                            if (token == QXmlStreamReader::StartElement && PIParser->name()=="FAM" ) Q->bindValue(":last_name" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && PIParser->name()=="IM") Q->bindValue(":name" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && PIParser->name()=="OT") Q->bindValue(":second_name" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && PIParser->name()=="W") Q->bindValue(":gender" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && PIParser->name()=="DR") Q->bindValue(":birth_date" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));

                                            if (Version == "3.0" && token == QXmlStreamReader::StartElement && PIParser->name()=="FAM_P")Q->bindValue(":p_last_name" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (Version == "3.0" && token == QXmlStreamReader::StartElement && PIParser->name()=="IM_P")Q->bindValue(":p_name" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if ( Version == "3.0" &&token == QXmlStreamReader::StartElement && PIParser->name()=="OT_P")Q->bindValue(":p_second_name" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (Version == "3.0" && token == QXmlStreamReader::StartElement && PIParser->name()=="W_P") Q->bindValue(":p_gender" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (Version == "3.0" && token == QXmlStreamReader::StartElement && PIParser->name()=="DR_P")Q->bindValue(":p_birth_date" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (Version == "2.1" && token == QXmlStreamReader::StartElement && PIParser->name()=="DOST") PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                            if (Version == "2.1" &&token == QXmlStreamReader::StartElement && PIParser->name()=="DOST_P") PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                            if (Version == "2.1" && PIParser->name()=="TEL") PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                            if ( PIParser->name()=="MR") Q->bindValue(":birth_place" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && PIParser->name()=="DOCTYPE")Q2->bindValue(":document_type" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && PIParser->name()=="DOCSER") Q2->bindValue(":document_series" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && PIParser->name()=="DOCNUM") Q2->bindValue(":document_serial_number" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && PIParser->name()=="SNILS") Q2->bindValue(":snils" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && PIParser->name()=="OKATOG")Q2->bindValue(":okato_g" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && PIParser->name()=="OKATOP")Q2->bindValue(":okato_p" , PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && (PIParser->name()=="COMENTP" || PIParser->name()=="COMENTZ") ) PIParser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);

                                 }

                               RegDBQuery->bindValue(":fam" , Q->boundValue(":last_name"));
                               RegDBQuery->bindValue(":im" , Q->boundValue(":name"));
                               RegDBQuery->bindValue(":ot" , Q->boundValue(":second_name"));
                               RegDBQuery->bindValue(":dr" , Q->boundValue(":birth_date"));
                               RegDBQuery->bindValue(":ss" , Q2->boundValue(":snils"));
                               RegDBQuery->bindValue(":docs" , Q2->boundValue(":document_series"));
                               RegDBQuery->bindValue(":docn" , Q2->boundValue(":document_serial_number"));
//                             // RegDBQuery->exec();
//                               while(RegDBQuery->next())
//                               {
//                                   //qDebug()<<RegDBQuery->value(0).toInt();
//                                   Q->bindValue(":reg_id" , RegDBQuery->value(0).toInt());
//                              }
                                Q->exec();
                                Q2->exec();

                              //  QtConcurrent::run(this , SQLQueryProcessor::CommitQueries , Q , Q2 , RegDBQuery );
                               // int T = Time.elapsed();

                        //qDebug()<<"Done in: "<<T/60000<<" minutes"<<(T%60000)/1000<<" seconds";
                              }
                        }
                 }
        }

    }
    else
    {
        qDebug()<<"Wrong file name: "<<LFile->fileName();
        return;
    }


qDebug()<<"Done reading personal info in: "<<Time.elapsed()/60000.;
LFile->close();
}

void SQLQueryProcessor::CommitQueries(QSqlQuery* Q, QSqlQuery* Q2, QSqlQuery* RegQ)
{
//   RegQ->bindValue(":fam" , Q->boundValue(":last_name"));
//   RegQ->bindValue(":im" , Q->boundValue(":name"));
//   RegQ->bindValue(":ot" , Q->boundValue(":second_name"));
//   RegQ->bindValue(":dr" , Q->boundValue(":birth_date"));
//   RegQ->bindValue(":ss" , Q2->boundValue(":snils"));
//   RegQ->bindValue(":docs" , Q2->boundValue(":document_series"));
//   RegQ->bindValue(":docn" , Q2->boundValue(":document_serial_number"));


   RegQ->exec();
                               while(RegQ->next())
                               {
                                   //qDebug()<<RegQ->value(0).toInt();
                                   Q->bindValue(":reg_id" , RegQ->value(0).toInt());
                               }
    Q->exec();
    Q2->exec();

}

SQLQueryProcessor::~SQLQueryProcessor()
{
    LFile->close();
}
