#include <QtTest/QtTest>
#include <QtCore/QtCore>
#include <QDebug>
#include <QtSparql/QtSparql>
#include <iostream>
using namespace std;


/*
** This Utility class have implementation to create, delete and show a table with ten records for TRACKER
** and VIRTUOSO backend support.
** it also have implementation to get the availble backend names support by QSPARQL.
*/

class SparqlApiTestsUtilities
{
public:
    QStringList backends;
    QStringList availablebackends()
    {
        // Locals.
        //QStringList availBackends;
        if (backends.count() == 0) {
            // Tracker
            {
                QString sparql_backend_name("QTRACKER");
                QSparqlConnection conn(sparql_backend_name);
                if(conn.isValid())
                    backends << sparql_backend_name;
            }
            // Tracker Direct
            {
                QString sparql_backend_name("QTRACKER_DIRECT");
                QSparqlConnection conn(sparql_backend_name);
                if(conn.isValid())
                    backends << sparql_backend_name;
            }
            // Virtuoso
            {
                QString sparql_backend_name("QVIRTUOSO");
                QString def_virtuoso_db("DRIVER=/usr/lib/odbc/virtodbc_r.so");
                QString def_virtuoso_host("localhost");
                int def_virtuoso_port(8890);

                QSparqlConnectionOptions options_virtuoso;
                options_virtuoso.setDatabaseName(def_virtuoso_db);
                options_virtuoso.setHostName(def_virtuoso_host);
                options_virtuoso.setPort(def_virtuoso_port);
                QSparqlConnection conn(sparql_backend_name, options_virtuoso);

                if(conn.isValid())
                    backends << sparql_backend_name;
            }
            // Endpoint.
            {
                QString sparql_backend_name("QSPARQL_ENDPOINT");
                QString def_dbpedia_host("dbpedia.org");
                int def_dbpedia_port(8890);

                QSparqlConnectionOptions options_dbpedia;
                options_dbpedia.setHostName(def_dbpedia_host);
                options_dbpedia.setPort(def_dbpedia_port);
                QSparqlConnection conn(sparql_backend_name, options_dbpedia);

                // run a query to make sure we can connect using the endpoint
                // driver but adding it
                QSparqlQuery selectQuery("SELECT ?Object "
                             "{ <http://dbpedia.org/resource/The_Beatles> "
                             "<http://dbpedia.org/property/currentMembers> ?Object }");
                QSparqlResult *result = conn.exec(selectQuery);
                result->waitForFinished();

                if(conn.isValid() && !result->hasError())
                    backends << sparql_backend_name;
                delete result;
            }
        }
        return backends;
    }//end of function


    /* Sample database creation. */
    bool createSampleDataBase(QString backendName)
    {
        if(availablebackends().contains(backendName) == false)
        {
            printf("\n Backend is not available");
            return false;
        }

        if (backendName == "QTRACKER" || backendName == "QTRACKER_DIRECT")
            return createTrackerDataBase();
        else if(backendName == "QVIRTUOSO")
            return createVirtuosoDataBase();
    }

    /* Sample database deletion. */
    bool deleteSampleDataBase(QString backendName)
    {
        if(availablebackends().contains(backendName) == false)
        {
            printf("\n Backend is not available");
            return false;
        }

        if (backendName == "QTRACKER" || backendName == "QTRACKER_DIRECT")
            return deleteTrackerDataBase();
        else if (backendName == "QVIRTUOSO")
            return deleteVirtuosoDataBase();
    }
    /* Display the data to user in table form. */
    void showDataBase(QString backendName)
    {
        if(availablebackends().contains(backendName) == false)
        {
            printf("\n Backend is not available");
            return;
        }

        QSparqlConnection conn(backendName);
        QSparqlResult* r;

        printf("\n***************Table Information**************");
        std::cout<<"\nBackend Name-> "<< backendName.toStdString();

        // Selecting the data from table
        QSparqlQuery q("select ?u ?ng ?nf{?u a nco:PersonContact; "
                       "nie:isLogicalPartOf <Tracker-DataBase> ;"
                       "nco:nameGiven ?ng;"
                       "nco:nameFamily ?nf.}");
        r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished(); // this test is syncronous only
        QCOMPARE(r->hasError(), false);


        // Printing the table information to end user
        printf("\nTable Name-> Tracker-DataBase");
        printf("\nRow Count->  %d",r->size());
        printf("\nTABLE:- PersonContact     nameGiven     nameFamily");
        printf("\n        -------------     ---------     ----------");
        int i=0;
        while (r->next()){
            std::cout<<"\n\t"<< r->binding(0).value().toString().toStdString();
            std::cout<<"\t"<< r->binding(1).value().toString().toStdString();
            std::cout<<"\t"<< r->binding(2).value().toString().toStdString();
            i++;
        }
        delete r;
        printf("\n\n*****************End of Table*****************\n\n");
    }

private slots:
    /* Creating table for QTRACKER backend support */
    bool createTrackerDataBase()
    {
        QSparqlConnection conn("QTRACKER");
        QSparqlResult* r;

        // Inserting 10 records of data into table
        for(int i=0; i<10; i++)
        {
            QString query= "insert { <contact";
            query= query.append(QString("%1").arg(i));
            query= query.append("> a nco:PersonContact; "
                                "nie:isLogicalPartOf <Tracker-DataBase> ;"
                                "nco:nameGiven \"personName");
            query= query.append(QString("%1").arg(i));
            query= query.append("\"; "
                                "nco:nameFamily \"familyName");
            query= query.append(QString("%1").arg(i));
            query= query.append("\".}");

            QSparqlQuery add(query, QSparqlQuery::InsertStatement);
            r = conn.exec(add);
            r->waitForFinished(); // this test is syncronous only
        }

        if(!r->hasError()){
            delete r;
            return true;
        }
        else{
            delete r;
            return false;
        }
    }

     /* Delete table for Tracker backend support */
    bool deleteTrackerDataBase()
    {
        // Delete the uri
        QSparqlConnection conn("QTRACKER");
        QSparqlResult *r, *r1;

        QSparqlQuery q("select ?u ?ng ?nf{?u a nco:PersonContact; "
                       "nie:isLogicalPartOf <Tracker-DataBase> ;"
                       "nco:nameGiven ?ng;"
                       "nco:nameFamily ?nf.}");
        r = conn.exec(q);
        r->waitForFinished(); // this test is syncronous only

        while(r->next())
        {
            QString query= "delete { <";
            query= query.append(r->binding(0).value().toString());
            query= query.append("> a rdfs:Resource. }");
            QSparqlQuery del(query,QSparqlQuery::DeleteStatement);
            r1 = conn.exec(del);
            r1->waitForFinished(); // this test is syncronous only
        }
        delete r1;

        if(!r->hasError()){
            delete r;
            return true;
        }
        else{
            delete r;
            return false;
        }
    }


   /*
    * TODO: The librieris are not avaible for Virtuoso.
   */
    /* Creating table for Virtuoso backend support. */
    bool createVirtuosoDataBase()
    {
        QSparqlConnectionOptions options;
        options.setDatabaseName("DRIVER=/usr/lib/odbc/virtodbc_r.so");
        QSparqlConnection conn("QVIRTUOSO", options);
        QSparqlResult* r;

        // Inserting 10 records of data into table
        for(int i=0; i<10; i++)
        {
            QString query= "prefix nco: <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#> "
                           "prefix nie: <http://www.semanticdesktop.org/ontologies/2007/01/19/nie#> "
                           "insert into <http://virtuoso/testgraph> "
                           "{ <contact";
            query= query.append(QString("%1").arg(i));
            query= query.append("> a nco:PersonContact; "
                                "nie:isLogicalPartOf <Virtuoso-DataBase> ;"
                                "nco:nameGiven \"personName");
            query= query.append(QString("%1").arg(i));
            query= query.append("\"; "
                                "nco:nameFamily \"familyName");
            query= query.append(QString("%1").arg(i));
            query= query.append("\".}");

            QSparqlQuery add(query, QSparqlQuery::InsertStatement);
            r = conn.exec(add);
            r->waitForFinished(); // this test is syncronous only
        }

        if(!r->hasError()){
            delete r;
            return true;
        }
        else{
            delete r;
            return false;
        }
    }


    /*
     * TODO: The librieris are not avaible for Virtuoso.
    */
    /* Delete table for Virtuoso backend support */
    bool deleteVirtuosoDataBase()
    {
        // Delete the uri
        QSparqlConnectionOptions options;
        options.setDatabaseName("DRIVER=/usr/lib/odbc/virtodbc_r.so");
        QSparqlConnection conn("QVIRTUOSO", options);
        QSparqlResult *r, *r1;

        QSparqlQuery q("select ?u ?ng ?nf{?u a nco:PersonContact; "
                       "nie:isLogicalPartOf <Virtuoso-DataBase> ;"
                       "nco:nameGiven ?ng;"
                       "nco:nameFamily ?nf.}");
        r = conn.exec(q);
        r->waitForFinished(); // this test is syncronous only

        while(r->next())
        {
            QString query= "delete { <";
            query= query.append(r->binding(0).value().toString());
            query= query.append("> a rdfs:Resource. }");
            QSparqlQuery del(query,QSparqlQuery::DeleteStatement);
            r1 = conn.exec(del);
            r1->waitForFinished(); // this test is syncronous only
        }
        delete r1;

        if(!r->hasError()){
            delete r;
            return true;
        }
        else{
            delete r;
            return false;
        }
    }
};



