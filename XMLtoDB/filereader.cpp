#include "filereader.h"
#include"sqlqueryprocessor.h"
#include<QRegExp>
#include<string>
#include<QVariant>
#include<QTime>


FileReader::FileReader()
{
    FileType = 1;
    CurrentFile = new QFile();

    db = QSqlDatabase::addDatabase("QPSQL", "ThreadConnection");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("postgres");
    db.setUserName("alewa");
    db.setPassword("EybrfkmysqGfhjkm");
    db.setPort(5432);
    qDebug()<<db.open();



    regdb = QSqlDatabase::addDatabase("QPSQL" , "RegisterConnection");
    regdb.setHostName("192.168.1.222");
    regdb.setDatabaseName("123");
    regdb.setUserName("postgres");
    regdb.setPassword("A3s4d5");
    regdb.setPort(5432);
    qDebug()<<regdb.open();
}

bool FileReader::Check(QString FName)
{

    QRegExp Regexp("[DTL][DFOPRSUV][TSM]{0,1}[0-9]{2,3}[0-9]{2}[TSM][0-9]{2,3}_[0-9]{0,6}.[Xx][Mm][Ll]");
    return Regexp.exactMatch(FName);

}

void FileReader::PersonalInfoRequest()
{
    QueryProcessor->PersInformationParser(PackageNumber);
}

bool FileReader::ReadFile(QString Path, QString FName)
{
    FileType = 0;
    CurrentFile = new QFile(Path+"\\"+FName);
    if(Check(FName))
    {
            qDebug()<<"Processing file: "+ FName;
             if(FName.startsWith('D')) FileType = 1;
             else
             if (FName.startsWith('T') ) FileType = 3;
             else
             if ( FName.startsWith('L') )
            {
                FileType = 2;
                LFilePath = Path;
                LFileName = FName;
            }
            PackageNumber = FName.split("_")[1];
            PackageNumber = PackageNumber.split(".")[0];
            QueryProcessor = new  SQLQueryProcessor( LFileName , LFilePath, db, regdb);
            return CurrentFile->exists();
    }
    else
    {
        qDebug()<<"Invalid name of file: "<<Path+"/"+FName;
        return false;
    }
}

void FileReader::ParseFile()
{
    QTime Time;
    Time.start();
    if(CurrentFile->open( QIODevice::ReadOnly | QIODevice::Text )) Parser = new QXmlStreamReader(CurrentFile);
    else
    {
        qDebug()<<"Error: file is not open";
        return;
    }

DocumentsQuery = new QSqlQuery(db);
ClosedCasesQuery = new QSqlQuery(db);
ServicesQuery = new QSqlQuery(db);
AppointmentsQuery = new QSqlQuery(db);
SanctionsQuery = new QSqlQuery(db);
CasesQuery = new QSqlQuery(db);
HiTechCasesQuery = new QSqlQuery(db);
BillQuery = new QSqlQuery(db);

HiTechCasesQuery->prepare("INSERT INTO hi_tech_cases (cl_case_id, hmp_type,  lpu_1,department,profile, profile_k, child, p_cel, tal_date, tal_number, tal_plan_date, history_id,date_in,"
                          "date_out, kd,  ds0, ds1, dn,code_mes1, code_mes2,reab,prvs,vers_spec, iddoct, payment_unit_count,  tariff, sum_m, ksg_smo, comment, payment_kind, case_id)"
                          "VALUES (:cl_case_id, :hmp_type,  :lpu_1, :department, :profile, :profile_k, :child, :p_cel, :tal_date, :tal_number, :tal_plan_date, :history_id, :date_in,"
                          ":date_out, :kd,  :ds0, :ds1, :dn, :code_mes1, :code_mes2, :reab, :prvs, :vers_spec, :iddoct, :payment_unit_count,  :tariff, :sum_m, :ksg_smo, :comment, :payment_kind, :case_id)");

CasesQuery->prepare("INSERT INTO cases (cl_case_id, case_id, lpu_1, history_number, treatment_start_date, treatment_stop_date, ds1, ds1_pr, pr_d_n, dn, payment_unit_count, tariff, sum_m, comment, payment_kind)"
                    "VALUES (:cl_case_id, :case_id,  :lpu_1, :history_number, :treatment_start_date, :treatment_stop_date, :ds1, :ds1_pr, :pr_d_n, :dn, :payment_unit_count, :tariff, :sum_m, :comment, :payment_kind)");

SanctionsQuery->prepare("INSERT INTO sanctions (cl_case_id, case_id, s_code, s_sum, s_org, s_tip, s_osn, s_com, s_ist) VALUES (:cl_case_id, :case_id, :s_code, :s_sum, :s_org, :s_tip, :s_osn, :s_com, :s_ist)");

AppointmentsQuery->prepare("INSERT INTO appointments (cl_case_id , case_id , appoint_id, appoint_r , appoint_sp) VALUES (:cl_case_id , :case_id , :appoint_id, :appoint_r , :appoint_sp)");

ServicesQuery->prepare("INSERT INTO services(cl_case_id, case_id, idserv, lpu, lpu_1, serv_date_in, serv_date_out, p_refuse, serv_code, tariff, sumv_serv, code_md, npl, comment , department, profile, ds, vid_vme, child, prvs, serv_count)"
                       "VALUES (:cl_case_id, :case_id, :idserv, :lpu, :lpu_1, :serv_date_in, :serv_date_out, :p_refuse, :serv_code, :tariff, :sumv_serv, :code_md, :npl, :comment, :department, :profile, :ds, :vid_vme, :child, :prvs, :serv_count)");

ClosedCasesQuery->prepare("INSERT INTO closed_cases (cl_case_id, pacient_id, med_help_type, lpu, vbr, case_in_date, case_out_date, p_refuse, rslt_d, kd_z, vnov_m,"
                          " special_case, vb_p, idsp, sumv, payment, sump, sanction_it, usl_ok, for_help, npr_mo, npr_date, rslt, ishod) "
                           "VALUES (:cl_case_id, :pacient_id, :med_help_type, :lpu, :vbr, :case_in_date, :case_out_date, :p_refuse, :rslt_d, :kd_z, :vnov_m,"
                          " :special_case, :vb_p, :idsp, :sumv, :payment, :sump, :sanction_it, :usl_ok, :for_help, :npr_mo, :npr_date, :rslt, :ishod) ");

DocumentsQuery->prepare("UPDATE documents SET (okato_st, v_polis, polis_series, polis_serial_number, smo, smo_ogrn, smo_ok, smo_nam, inv , new_born, new_born_w) = "
                        "(:okato_st, :v_polis, :polis_series, :polis_serial_number, :smo, smo_ogrn, :smo_ok, :smo_nam, :inv , :new_born, :new_born_w )"
                        "WHERE pacient_id = :pacient_id");

BillQuery->prepare("INSERT INTO bill (code , code_mo, year, month , bill_date, payer, summ_v, comments, summ_p, sanct_mek, sanct_mee, sanct_ekmp, sanct_org, disp, bill_number) "
                   "VALUES (:code , :code_mo, :year, :month , :bill_date, :payer, :summ_v, :comments, :summ_p, :sanct_mek, :sanct_mee, :sanct_ekmp, :sanct_org, :disp, :bill_number)");

if(FileType == 1 || FileType == 3 )
    while (!Parser->atEnd() && !Parser->hasError())
       {

           QXmlStreamReader::TokenType token = Parser->readNext();
           if (token == QXmlStreamReader::StartDocument)
               continue;
           if (token == QXmlStreamReader::StartElement && Parser->name()=="ZL_LIST")
           {

               while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="ZL_LIST")))
               {
                   if(token == QXmlStreamReader::StartElement && Parser->name()=="ZGLV")
                   {
                       while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="ZGLV")))
                       {
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="VERSION") Version = Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="DATA") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="FILENAME") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="SD_Z") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                       }
                    }

                   if(Version == "3.0" )
                   {
                   if(token == QXmlStreamReader::StartElement && Parser->name()=="SCHET")
                   {

                       while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="SCHET")))
                       {
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE") BillQuery->bindValue(":code",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE_MO") BillQuery->bindValue(":code_mo", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="YEAR") BillQuery->bindValue(":year",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="MONTH") BillQuery->bindValue(":month",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="NSCHET") BillQuery->bindValue(":bill_number",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="DSCHET") BillQuery->bindValue(":bill_date",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="PLAT") BillQuery->bindValue(":payer",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="SUMMAV") BillQuery->bindValue(":summa_v",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="COMENTS") BillQuery->bindValue(":comments",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="SUMMAP") BillQuery->bindValue(":summa_p",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="SANK_MEK") BillQuery->bindValue(":sanct_mek",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="SANK_MEE") BillQuery->bindValue(":sanct_mee",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="SANK_EKMP") BillQuery->bindValue(":sanct_ekmp",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="SANK_ORG") BillQuery->bindValue(":sanct_org",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="DISP") BillQuery->bindValue(":disp",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                       }
                       BillQuery->exec();
                    }

                   if(token == QXmlStreamReader::StartElement && Parser->name()=="ZAP")
                   {

                       while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="ZAP")))
                       {


                           if (token == QXmlStreamReader::StartElement && Parser->name()=="N_ZAP") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                           if (token == QXmlStreamReader::StartElement && Parser->name()=="PR_NOV") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);

                           if(token == QXmlStreamReader::StartElement && Parser->name()=="PACIENT")
                           {
                               while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="PACIENT")))
                               {

                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="ID_PAC")
                                   {
                                   QString str = Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                    DocumentsQuery->bindValue( ":pacient_id" , str);                                    
                                    ClosedCasesQuery->bindValue(":pacient_id", str);                                   
                                   }

                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="VPOLIS") DocumentsQuery->bindValue( ":v_polis" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="SPOLIS") DocumentsQuery->bindValue( ":polis_series" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="NPOLIS") DocumentsQuery->bindValue( ":polis_serial_number" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if ( token == QXmlStreamReader::StartElement && Parser->name()=="ST_OKATO") DocumentsQuery->bindValue( ":okato_st" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="SMO") DocumentsQuery->bindValue( ":smo" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="SMO_OGRN") DocumentsQuery->bindValue( ":smo_ogrn" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="SMO_OK") DocumentsQuery->bindValue( ":smo_ok" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="SMO_NAM") DocumentsQuery->bindValue( ":smo_nam" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="INV") DocumentsQuery->bindValue( ":inv" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="NOVOR") DocumentsQuery->bindValue( ":new_born" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="VNOV_D") DocumentsQuery->bindValue( ":new_born_w" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));

                               }

                               DocumentsQuery->exec();                           
                           }

                           if(token == QXmlStreamReader::StartElement && ((Version == "3.0" && Parser->name()=="Z_SL") ))
                           {
                                while( Parser->readNextStartElement() && ( !Parser->isEndElement() && !(Parser->name()=="Z_SL") ))
                               {

                                    //qDebug()<<Parser->name();
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="IDCASE")
                                   {
                                       QString temp = Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                       ClosedCasesQuery->bindValue(":cl_case_id" , temp);
                                       ServicesQuery->bindValue(":cl_case_id", temp);
                                       AppointmentsQuery->bindValue(":cl_case_id" , temp);
                                       SanctionsQuery->bindValue(":cl_case_id", temp);
                                       CasesQuery->bindValue(":cl_case_id", temp);
                                       if(FileType == 3) HiTechCasesQuery->bindValue(":cl_case_id" , temp);

                                   }
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="VIDPOM"){ ClosedCasesQuery->bindValue(":med_help_type" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="USL_OK") {ClosedCasesQuery->bindValue(":usl_ok" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="FOR_POM"){ ClosedCasesQuery->bindValue(":for_help" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="NPR_MO"){ ClosedCasesQuery->bindValue(":npr_mo" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="NPR_DATE"){ClosedCasesQuery->bindValue(":npr_date" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="LPU"){ ClosedCasesQuery->bindValue(":lpu" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="LPU_1"){ Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="VBR"){ ClosedCasesQuery->bindValue(":vbr" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="DATE_Z_1") {ClosedCasesQuery->bindValue(":case_in_date" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="DATE_Z_2") {ClosedCasesQuery->bindValue(":case_out_date" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="P_OTK") {ClosedCasesQuery->bindValue(":p_refuse" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="RSLT_D") {ClosedCasesQuery->bindValue(":rslt_d" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="KD_Z")  {ClosedCasesQuery->bindValue(":kd_z" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="VNOV_M"){ ClosedCasesQuery->bindValue(":vnov_m" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="RSLT"){ClosedCasesQuery->bindValue(":rslt" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="ISHOD"){ ClosedCasesQuery->bindValue(":ishod" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="OS_SLUCH"){ClosedCasesQuery->bindValue(":special_case" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="VB_P"){ClosedCasesQuery->bindValue(":vb_p" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}

                                   if(Version == "3.0" && (token == QXmlStreamReader::StartElement && Parser->name()=="SL"))
                                   {
                                       while(Parser->readNextStartElement() && ((!Parser->isEndElement() && !(Parser->name()=="SL"))))
                                       {
                                           if (token == QXmlStreamReader::StartElement && Parser->name()=="SL_ID")
                                           {
                                               QString temp2 = Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                               AppointmentsQuery->bindValue(":case_id" , temp2);
                                               SanctionsQuery->bindValue(":case_id" , temp2);
                                               CasesQuery->bindValue(":case_id", temp2);
                                                if(FileType == 3) HiTechCasesQuery->bindValue(":case_id" , temp2);
                                           }


                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="LPU_1"){ CasesQuery->bindValue(":lpu_1", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="NHISTORY"){ CasesQuery->bindValue(":history_number" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="DATE_1"){ CasesQuery->bindValue(":treatment_start_date", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="DATE_2"){ CasesQuery->bindValue(":treatment_stop_date", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="DS1") {CasesQuery->bindValue(":ds1", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="DS1_PR") {CasesQuery->bindValue(":ds1_pr", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="PR_D_N"){ CasesQuery->bindValue(":pr_d_n", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="VID_HMP") {HiTechCasesQuery->bindValue(":hmp_type" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="METOD_HMP"){HiTechCasesQuery->bindValue(":hmp_metod" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="PODR") {HiTechCasesQuery->bindValue(":department" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="PROFIL") {HiTechCasesQuery->bindValue(":profile" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="PROFIL_K") {HiTechCasesQuery->bindValue(":profile_k" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="DET") {HiTechCasesQuery->bindValue(":child" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="P_CEL") {HiTechCasesQuery->bindValue(":p_cel" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="TAL_D") {HiTechCasesQuery->bindValue(":tal_date" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="TAL_NUM") {HiTechCasesQuery->bindValue(":tal_number" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="TAL_P") {HiTechCasesQuery->bindValue(":tal_plan_date" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="NHISTORY") {HiTechCasesQuery->bindValue(":history_id" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="KD") {HiTechCasesQuery->bindValue(":kd" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="DS0") {HiTechCasesQuery->bindValue(":ds0" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="DS2") {HiTechCasesQuery->bindValue(":ds1" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE_MES1") {HiTechCasesQuery->bindValue(":code_mes1" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE_MES2") {HiTechCasesQuery->bindValue(":code_mes2" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="REAB") {HiTechCasesQuery->bindValue(":reab" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="PRVS") {HiTechCasesQuery->bindValue(":prvs" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="VERS_SPEC") {HiTechCasesQuery->bindValue(":vers_spec" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="IDDOKT") {HiTechCasesQuery->bindValue(":iddoct" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="KSG_SMO") {HiTechCasesQuery->bindValue(":ksg_smo" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="PAYMENT_KIND"){ HiTechCasesQuery->bindValue(":payment_kind" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}

                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="NAZR") {AppointmentsQuery->bindValue(":appoint_r",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                           if(token == QXmlStreamReader::StartElement && Parser->name()=="NAZ")
                                           {
                                               while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="NAZ")))
                                               {
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="NAZ_N") AppointmentsQuery->bindValue(":appoint_id", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="NAZ_R") AppointmentsQuery->bindValue(":appoint_r",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="NAZ_SP") AppointmentsQuery->bindValue(":appoint_sp",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                               }
                                               AppointmentsQuery->exec();
                                           }

                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="DN") CasesQuery->bindValue(":dn", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="ED_COL")CasesQuery->bindValue(":payment_unit_count", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="TARIF") CasesQuery->bindValue(":tariff", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="SUM_M") CasesQuery->bindValue(":sum_m", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="SANK_IT") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                            if (token == QXmlStreamReader::StartElement && Parser->name()=="SANK")
                                            {
                                               while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="SANK")))
                                               {
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="S_CODE") SanctionsQuery->bindValue(":s_code" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="S_SUM") SanctionsQuery->bindValue(":s_sum" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="S_ORG") SanctionsQuery->bindValue(":s_org" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="S_TIP") SanctionsQuery->bindValue(":s_tip" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="S_OSN") SanctionsQuery->bindValue(":s_osn" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="S_COM") SanctionsQuery->bindValue(":s_com" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="S_IST") SanctionsQuery->bindValue(":s_ist" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));

                                               }
                                               SanctionsQuery->exec();

                                           }
                                           if(token == QXmlStreamReader::StartElement && Parser->name()=="USL")
                                           {
                                               while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="USL")))
                                               {

                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="IDSERV")  ServicesQuery->bindValue(":idserv", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="LPU") ServicesQuery->bindValue(":lpu", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="LPU_1") ServicesQuery->bindValue(":lpu_1", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="DATE_IN") ServicesQuery->bindValue(":serv_date_in", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="DATE_OUT") ServicesQuery->bindValue(":serv_date_out", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="P_OTK") ServicesQuery->bindValue(":p_refuse", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE_USL") ServicesQuery->bindValue(":serv_code", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="TARIF") ServicesQuery->bindValue(":tariff", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="SUMV_USL") ServicesQuery->bindValue(":sumv_serv", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE_MD") ServicesQuery->bindValue(":code_md", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="NPL") ServicesQuery->bindValue(":npl", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="COMENTU") ServicesQuery->bindValue(":comment", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="PODR") ServicesQuery->bindValue(":department", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="PROFIL") ServicesQuery->bindValue(":profile", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="DS") ServicesQuery->bindValue("ds", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="VID_VME") ServicesQuery->bindValue(":vid_vme", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="DET") ServicesQuery->bindValue(":child", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="PRVS") ServicesQuery->bindValue(":prvs", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="KOL_USL") ServicesQuery->bindValue(":serv_count", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));

                                               }

                                               ServicesQuery->exec();
                                           }

                                           if (token == QXmlStreamReader::StartElement && Parser->name()=="COMENTSL") {CasesQuery->bindValue(":comment",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));}
                                           if(token == QXmlStreamReader::StartElement && Parser->name()=="DS2_N")
                                           {
                                               while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="DS2_N")))
                                               {
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="DS2") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="DS2_PR") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                  }
                                           }
                                           if (token == QXmlStreamReader::StartElement && Parser->name()=="PAYMENT_KIND") CasesQuery->bindValue(":payment_kind",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                       }
                                       if(FileType == 1 )
                                       {
                                           CasesQuery->exec();
                                       }
                                       if(FileType == 3 ) HiTechCasesQuery->exec();
                                   }
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="IDSP") ClosedCasesQuery->bindValue(":idsp" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="SUMV") ClosedCasesQuery->bindValue(":sumv" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="OPLATA") ClosedCasesQuery->bindValue(":payment" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="SUMP") ClosedCasesQuery->bindValue(":sump" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                   if (token == QXmlStreamReader::StartElement && Parser->name()=="SANK_IT") ClosedCasesQuery->bindValue(":sanction_it" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                               }
                               ClosedCasesQuery->exec();
                           }
                       }
                   }
                }

               else if(Version == "2.1")
               {
                   if(token == QXmlStreamReader::StartElement && Parser->name()=="SCHET")
                                      {
                                          while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="SCHET")))
                                          {

                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE") BillQuery->bindValue(":code",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE_MO") BillQuery->bindValue(":code_mo", Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="YEAR") BillQuery->bindValue(":year",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="MONTH") BillQuery->bindValue(":month",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="NSCHET") BillQuery->bindValue(":bill_number",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="DSCHET") BillQuery->bindValue(":bill_date",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="PLAT") BillQuery->bindValue(":payer",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SUMMAV") BillQuery->bindValue(":summa_v",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="COMENTS") BillQuery->bindValue(":comments",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SUMMAP") BillQuery->bindValue(":summa_p",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SANK_MEK") BillQuery->bindValue(":sanct_mek",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SANK_MEE") BillQuery->bindValue(":sanct_mee",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SANK_EKMP") BillQuery->bindValue(":sanct_ekmp",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="DISP") BillQuery->bindValue(":disp",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SANK_ORG") BillQuery->bindValue(":sanct_org",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="POL_COL") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="POL_SUMPF") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SMP_COL") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SMP_SUMPF") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="FS_COL") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="FS_SUMPF") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                          }
                                          BillQuery->exec();
                                     }
                                              if(token == QXmlStreamReader::StartElement && Parser->name()=="ZAP")
                                              {

                                                  while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="ZAP")))
                                                  {
                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="N_ZAP") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="PR_NOV") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);

                                                      if(token == QXmlStreamReader::StartElement && Parser->name()=="PACIENT")
                                                      {
                                                          while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="PACIENT")))
                                                          {

                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="ID_PAC")
                                                              {
                                                                  QString str = Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                                   DocumentsQuery->bindValue( ":pacient_id" , str);
                                                                   ClosedCasesQuery->bindValue(":pacient_id", str);
                                                                  }

                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="VPOLIS") DocumentsQuery->bindValue( ":v_polis" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SPOLIS") DocumentsQuery->bindValue( ":polis_series" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="NPOLIS") DocumentsQuery->bindValue(":polis_serial_number",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="ST_OKATO") DocumentsQuery->bindValue(":okato_st",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SMO") DocumentsQuery->bindValue(":smo",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SMO_OGRN") DocumentsQuery->bindValue(":smo_ogrn",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SMO_OK") DocumentsQuery->bindValue(":smo_ok",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SMO_NAM") DocumentsQuery->bindValue(":smo_nam",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="INV") DocumentsQuery->bindValue(":inv",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="NOVOR") DocumentsQuery->bindValue(":new_born",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="VNOV_D")  DocumentsQuery->bindValue(":new_born_w",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                          }
                                                          DocumentsQuery->exec();
                                                      }

                                                      if(token == QXmlStreamReader::StartElement && Parser->name()=="SLUCH")
                                                      {

                                                          while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="SLUCH")))
                                                          {
                                                                //qDebug()<<Parser->name();
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="IDCASE")
                                                              {
                                                                  QString temp = Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                                  ClosedCasesQuery->bindValue(":cl_case_id" , temp);
                                                                  ServicesQuery->bindValue(":cl_case_id", temp);
                                                                  AppointmentsQuery->bindValue(":cl_case_id" , temp);
                                                                  SanctionsQuery->bindValue(":cl_case_id", temp);
                                                                  CasesQuery->bindValue(":cl_case_id", temp);
                                                                  CasesQuery->bindValue(":case_id", temp);
                                                                  if(FileType == 3) HiTechCasesQuery->bindValue(":cl_case_id" , temp);
                                                              }

                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="VIDPOM") ClosedCasesQuery->bindValue(":med_help_type" , Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="LPU") ClosedCasesQuery->bindValue(":lpu",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="LPU_1") CasesQuery->bindValue(":lpu_1",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="VBR") ClosedCasesQuery->bindValue(":vbr",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="NHISTORY") CasesQuery->bindValue(":history_number",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="P_OTK") ClosedCasesQuery->bindValue(":p_refuse",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="DATE_1") ClosedCasesQuery->bindValue(":case_in_date",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="DATE_2") ClosedCasesQuery->bindValue(":case_out_date",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="DS1") CasesQuery->bindValue(":ds1",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="DS1_PR") CasesQuery->bindValue(":ds1_pr",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="RSLT_D") ClosedCasesQuery->bindValue(":rslt_d",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="NAZR") AppointmentsQuery->bindValue(":appoint_r",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="NAZ_SP") AppointmentsQuery->bindValue(":appoint_sp",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="NAZ_V") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="NAZ_PMP") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="NAZ_PK") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="PR_D_N") CasesQuery->bindValue(":pr_d_n",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="IDSP") ClosedCasesQuery->bindValue(":idsp",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="ED_COL") CasesQuery->bindValue(":payment_unit_count",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="TARIF") CasesQuery->bindValue(":tariff",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SUMV") ClosedCasesQuery->bindValue(":sumv",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="OPLATA")ClosedCasesQuery->bindValue(":payment",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SUMP") ClosedCasesQuery->bindValue(":sump",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="SANK_IT") ClosedCasesQuery->bindValue(":sanction_it",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="KSG_MO") HiTechCasesQuery->bindValue(":ksg_smo",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));

                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="USL_OK") ClosedCasesQuery->bindValue(":usl_ok",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="FOR_POM") ClosedCasesQuery->bindValue(":for_help",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="VID_HMP") HiTechCasesQuery->bindValue(":hmp_type",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="METOD_HMP") HiTechCasesQuery->bindValue(":hmp_metod",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="NPR_MO") ClosedCasesQuery->bindValue(":npr_mo",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="EXTR") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="PODR") HiTechCasesQuery->bindValue(":department",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="PROFIL") HiTechCasesQuery->bindValue(":profile",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="DET") HiTechCasesQuery->bindValue(":child",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="P_PER") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="TAL_D") HiTechCasesQuery->bindValue(":tal_date" ,Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="TAL_P") HiTechCasesQuery->bindValue(":tal_plan_date",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="DS0") HiTechCasesQuery->bindValue(":ds0",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="DS3") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE_MES1") HiTechCasesQuery->bindValue(":code_mes1",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE_MES2") HiTechCasesQuery->bindValue(":code_mes2",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="RSLT") ClosedCasesQuery->bindValue(":rslt",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="ISHOD") ClosedCasesQuery->bindValue(":ishod",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="PRVS") HiTechCasesQuery->bindValue(":prvs",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="VERS_SPEC") HiTechCasesQuery->bindValue(":vers_spec",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="IDDOKT") HiTechCasesQuery->bindValue(":iddoct",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="DS2_N")
                                                               {
                                                                    if (token == QXmlStreamReader::StartElement && Parser->name()=="DS2") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                                     if (token == QXmlStreamReader::StartElement && Parser->name()=="DS2_PR") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                               }



                                                                if(token == QXmlStreamReader::StartElement && Parser->name()=="USL")
                                                              {
                                                                  while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="USL")))
                                                                  {
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="IDSERV") ServicesQuery->bindValue(":idserv",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="LPU") ServicesQuery->bindValue(":lpu",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="LPU_1") ServicesQuery->bindValue(":lpu_1",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="DATE_IN") ServicesQuery->bindValue(":serv_date_in",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="DATE_OUT") ServicesQuery->bindValue(":serv_date_out",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="P_OTK") ServicesQuery->bindValue(":p_refuse",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="DS") ServicesQuery->bindValue(":ds",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE_USL") ServicesQuery->bindValue(":serv_code",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="KOL_USL") ServicesQuery->bindValue(":serv_count",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="SUMV_USL") ServicesQuery->bindValue(":sumv_serv",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="TARIF") ServicesQuery->bindValue(":tariff",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));

                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="PRVS") ServicesQuery->bindValue(":sanct_org",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE_MD") ServicesQuery->bindValue(":prvs",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="NPL") ServicesQuery->bindValue(":npl",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                      if (token == QXmlStreamReader::StartElement && Parser->name()=="KSG_MO") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);

                                                                       if (token == QXmlStreamReader::StartElement && Parser->name()=="VID_VME") ServicesQuery->bindValue(":vid_vme",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                       if (token == QXmlStreamReader::StartElement && Parser->name()=="DET") ServicesQuery->bindValue(":child",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                       if (token == QXmlStreamReader::StartElement && Parser->name()=="COMENTU") ServicesQuery->bindValue(":comment",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                       if (token == QXmlStreamReader::StartElement && Parser->name()=="PODR") ServicesQuery->bindValue(":deoartment",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                                       if (token == QXmlStreamReader::StartElement && Parser->name()=="PROFIL") ServicesQuery->bindValue(":profile",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));

                                                                       if(token == QXmlStreamReader::StartElement && Parser->name()=="LEKPREP")
                                                                       {
                                                                           while(Parser->readNextStartElement() && (!Parser->isEndElement() && !(Parser->name()=="LEKPREP")))
                                                                           {
                                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="CODE_LP") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
                                                                               if (token == QXmlStreamReader::StartElement && Parser->name()=="NAME_LP") Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);

                                                                           }
                                                                       }
                                                                  }
                                                                  ServicesQuery->exec();

                                                              }
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="COMENTSL") CasesQuery->bindValue(":comment",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="PAYMENT_KIND" && FileType == 1)  CasesQuery->bindValue(":payment_kind",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                              if (token == QXmlStreamReader::StartElement && Parser->name()=="PAYMENT_KIND" && FileType == 3)  HiTechCasesQuery->bindValue(":payment_kind",Parser->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement));
                                                          }
                                                          ClosedCasesQuery->exec();
                                                           if(FileType == 1)CasesQuery->exec();
                                                          if(FileType == 3) HiTechCasesQuery->exec();

                                                      }
                                                  }
                                              }

                         }
               }
           }   //ZL_LIST
       }
CurrentFile->close();
}

QString FileReader::GetFileName()
{
    return CurrentFile->fileName();
}

QString FileReader::GetLFileName()
{
    return LFileName;
}

QString FileReader::GetLFilePath()
{
    return LFilePath;
}

void FileReader::addFileToPackage(QString File)
{
    Package.append(File);
    FileTypes.append(FileType);
}

QString FileReader::GetFileFromPackage(int index)
{
    if((index<Package.length()) && (index<FileTypes.length())) {FileType = FileTypes[index]; return Package[index]; }
    else
    {
        qDebug()<<"Package index out of bounds";
        return "";
    }

}

void FileReader::SetCurrentFile(QString FilePath)
{
    CurrentFile = new QFile(FilePath);

}

QStringList FileReader::GetPackage()
{
     return Package;
}

FileReader::~FileReader()
{
    CurrentFile->close();
    db.close();
    QueryProcessor->SQLQueryProcessor::~SQLQueryProcessor();

}


