/*
 * Copyright (C) 2010 Nokia Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <rtcom-benchmark.h>

RTComBenchmark::RTComBenchmark ()
{
    ;
}

void RTComBenchmark::initTestCase()
{
    // For running the test without installing the plugins. Should work in
    // normal and vpath builds.
    QCoreApplication::addLibraryPath("../../../../plugins");
}

void RTComBenchmark::test_rtcom_04 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?channel"
                  "?participant"
                  "nco:fullname(?participant)"
                  "?last_date"
                  "nie:plainTextContent(?last_message)"
                  "("
                        "SELECT COUNT(?message) AS message_count WHERE {"
                                "?message nmo:communicationChannel ?channel"
                        "}"
                  ")"
                  "("
                        "SELECT COUNT(?message) AS message_count WHERE {"
                                "?message nmo:communicationChannel ?channel ."
                                "?message nmo:isRead true"
                        "}"
                  ")"
                  "WHERE {"
                        "SELECT ?channel"
                               "?participant"
                               "?last_date"
                               "("
                                        "SELECT ?message WHERE {"
                                                "?message nmo:communicationChannel ?channel ."
                                                "?message nmo:sentDate ?date ."
                                                "} ORDER BY DESC (?date) LIMIT 1"
                               ") AS ?last_message"
                        "WHERE {"
                                "?channel a nmo:CommunicationChannel ."
                                "?channel nmo:lastMessageDate ?last_date ."
                                "?channel nmo:hasParticipant ?participant ."
                                "FILTER (?participant != nco:default-contact_me)"
                        "}"
                  "}");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }

    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?_channel ?_participant "
       "<http://www.semanticdesktop.org/ontologies/2007/03/22/nco#fullname>(?_participant) AS ?_fullname "
       "?_last_date "
       "( SELECT COUNT(?_message_1)"
         "AS ?_message_count_1 "
         "WHERE {"
            "?_message_1 <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#communicationChannel> ?_channel ."
         "}) AS ?_all_message_count "
       "( SELECT "
         "COUNT(?_message)"
         "AS ?_message_count "
         "WHERE {"
             "?_message <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#communicationChannel> ?_channel ."
             "?_message <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#isRead> \"true\"^^<http://www.w3.org/2001/XMLSchema#string> ."
       "}) AS ?_read_message_count "
     "<http://www.semanticdesktop.org/ontologies/2007/01/19/nie#plainTextContent>(?_last_message_1) "
  "AS ?_plain_text_content "
"WHERE"
"{"
  "{"
    "SELECT ?_channel ?_participant ?_last_date "
         "( SELECT ?_last_message "
  "WHERE {"
  ""
    "?_last_message <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#communicationChannel> ?_channel ."
    "?_last_message <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#sentDate> ?_date ."
  "}"
    "ORDER BY DESC(?_date)"
    "LIMIT 1)"
 ""
      "AS ?_last_message_1  ?_last_message"
    "WHERE"
    "{"
      "?_channel <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#CommunicationChannel> ."
      "?_channel <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#lastMessageDate> ?_last_date ."
      "?_channel <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#hasParticipant> ?_participant ."
      "FILTER((?_participant != <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#default-contact-me>)) ."
    "}"
  "}"
"}");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void RTComBenchmark::test_rtcom_01 ()
{

    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?msg ?date ?text ?contact"
        "WHERE {"
                "?msg nmo:communicationChannel <urn:channel:1> ;"
                "nmo:sentDate ?date ;"
                "nie:plainTextContent ?text ."
                "<urn:channel:1> nmo:hasParticipant ?contact ."
        "} ORDER BY DESC(?date) LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void RTComBenchmark::test_rtcom_05 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?msg ?date ?text ?contact"
        "WHERE {"
                "?msg nmo:communicationChannel <urn:channel:1> ;"
                "nmo:sentDate ?date ;"
                "nie:plainTextContent ?text ;"
                "nmo:from [ nco:hasIMAddress ?fromAddress ] ."
                "<urn:channel:1> nmo:hasParticipant ?contact ."
                "?contact nco:hasIMAddress ?fromAddress ."
        "} ORDER BY DESC(?date) LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void RTComBenchmark::test_rtcom_02 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?message ?date ?from ?to"
             "rdf:type(?message)"
             "tracker:coalesce(fn:concat(nco:nameGiven(?contact), ' ', nco:nameFamily(?contact)), nco:nickname(?contact))"
             "nco:contactUID(?contact)"
             "nmo:communicationChannel(?message)"
             "nmo:isSent(?message)"
             "nmo:isDraft(?message)"
             "nmo:isRead(?message)"
             "nmo:isAnswered(?message)"
             "nmo:isDeleted(?message)"
             "nmo:messageId(?message)"
             "nmo:smsId(?message)"
             "nmo:sentDate(?message)"
             "nmo:receivedDate(?message)"
             "nie:contentLastModified(?message)"
             "nmo:messageSubject(?message)"
             "nie:plainTextContent(?message)"
             "nmo:deliveryStatus(?message)"
             "nmo:reportDelivery(?message)"
             "nie:url(?message)"
             "nfo:fileName(nmo:fromVCard(?message))"
             "rdfs:label(nmo:fromVCard(?message))"
             "nfo:fileName(nmo:toVCard(?message))"
             "rdfs:label(nmo:toVCard(?message))"
             "nmo:encoding(?message)"
             "nie:characterSet(?message)"
             "nie:language(?message)"
        "WHERE"
        "{"
          "SELECT"
             "?message ?date ?from ?to"
             "(SELECT ?contact"
              "WHERE"
              "{"
                  "{"
                    "<urn:channel:1> nmo:hasParticipant ?participant ."
                    "?contact a nco:PersonContact ."
                    "?participant nco:hasIMAddress ?imaddress ."
                    "?contact nco:hasIMAddress ?imaddress ."
                  "}"
                  "UNION"
                  "{"
                    "<urn:channel:1> nmo:hasParticipant ?participant ."
                    "?contact a nco:PersonContact ."
                    "?participant nco:hasPhoneNumber ?participantNumber ."
                    "?participantNumber maemo:localPhoneNumber ?number ."
                    "?contact nco:hasPhoneNumber ?contactNumber ."
                    "?contactNumber maemo:localPhoneNumber ?number ."
                  "}"
              "}"
             ") AS ?contact"
          "WHERE"
          "{"
            "?message a nmo:Message ."
            "?message nmo:isDraft false ."
            "?message nmo:isDeleted false ."
            "?message nmo:sentDate ?date ."
            "?message nmo:from ?fromContact ."
            "?message nmo:to ?toContact ."
            "?fromContact nco:hasContactMedium ?from ."
            "?toContact nco:hasContactMedium ?to ."
            "?message nmo:communicationChannel <urn:channel:1> ."
          "}"
          "ORDER BY DESC(?date)"
        "}"
        "LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void RTComBenchmark::test_rtcom_06 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?channel"
               "?subject"
               "nie:generator(?channel)"
               "tracker:coalesce(fn:concat(nco:nameGiven(?contact), "
                                          "' ',"
                                          "nco:nameFamily(?contact)),"
                                          "nco:nickname(?contact)) AS ?contactName "
               "nco:contactUID(?contact) AS ?contactUID"
               "?lastDate"
               "?lastMessage nie:plainTextContent(?lastMessage)"
               "nfo:fileName(nmo:fromVCard(?lastMessage))"
               "rdfs:label(nmo:fromVCard(?lastMessage))"
               "( SELECT COUNT(?message) AS ?total_messages "
                 "WHERE { ?message nmo:communicationChannel ?channel . })"
               "( SELECT COUNT(?message) AS ?total_unread"
                 "WHERE { ?message nmo:communicationChannel ?channel . ?message nmo:isRead false  .})"
               "( SELECT COUNT(?message) AS ?_total_sent"
                 "WHERE { ?message nmo:communicationChannel ?channel . ?message nmo:isSent true . })"
        "WHERE {"
                "SELECT ?channel  ?subject  ?lastDate"
                       "( SELECT ?message"
                         "WHERE {"
                                "?message nmo:communicationChannel ?channel ."
                                "?message nmo:sentDate ?sentDate ."
                         "} ORDER BY DESC(?sentDate) LIMIT 1"
                       ") AS ?lastMessage"
                       "( SELECT ?contact"
                         "WHERE {"
                                "{"
                                        "?channel nmo:hasParticipant ?participant ."
                                        "?contact a nco:PersonContact ."
                                        "?participant nco:hasIMAddress ?imaddress ."
                                        "?contact nco:hasIMAddress ?imaddress ."
                                "}"
                                "UNION"
                                "{"
                                        "?channel nmo:hasParticipant ?participant ."
                                        "?contact a nco:PersonContact ."
                                        "?participant nco:hasPhoneNumber ?participantNumber ."
                                        "?number maemo:localPhoneNumber ?localNumber ."
                                        "?contact nco:hasPhoneNumber ?contactNumber ."
                                        "?contactNumber maemo:localPhoneNumber ?localNumber ."
                                "}"
                         "}"
                        ") AS ?contact"
                "WHERE {"
                        "?channel a nmo:CommunicationChannel ."
                        "?channel nie:subject ?subject ."
                        "?channel nmo:lastMessageDate ?lastDate ."
                "}"
        "}"
        "ORDER BY DESC(?lastDate) LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void RTComBenchmark::test_rtcom_03 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?call ?date ?from ?to"
             "rdf:type(?call)"
             "nmo:isSent(?call)"
             "nmo:isAnswered(?call)"
             "nmo:isRead(?call)"
             "nmo:sentDate(?call)"
             "nmo:receivedDate(?call)"
             "nmo:duration(?call)"
             "nie:contentLastModified(?call)"
             "(SELECT ?contact"
              "WHERE"
              "{"
                  "{"
                      "?contact a nco:PersonContact ."
                      "?contact nco:contactUID ?contactId ."
                      "{"
                        "?call nmo:to ?address ."
                      "}"
                      "UNION"
                      "{"
                        "?call nmo:from ?address ."
                      "}"
                    "?address nco:hasIMAddress ?imaddress ."
                    "?contact nco:hasIMAddress ?imaddress ."
                  "}"
                  "UNION"
                  "{"
                      "?contact a nco:PersonContact ."
                      "?contact nco:contactUID ?contactId ."
                      "{"
                        "?call nmo:to ?address ."
                      "}"
                      "UNION"
                      "{"
                        "?call nmo:from ?address ."
                      "}"
                    "?address nco:hasPhoneNumber ?addressNumber ."
                    "?addressNumber maemo:localPhoneNumber ?number ."
                    "?contact nco:hasPhoneNumber ?contactNumber ."
                    "?contactNumber maemo:localPhoneNumber ?number ."
                  "}"
              "})"
        "WHERE"
        "{"
          "{"
            "?call a nmo:Call ."
            "?call nmo:sentDate ?date ."
            "?call nmo:from ?fromContact ."
            "?call nmo:to ?toContact ."
            "?fromContact nco:hasContactMedium ?from ."
            "?toContact nco:hasContactMedium ?to ."
          "}"
        "}"
        "ORDER BY DESC(?date) LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

QTEST_MAIN(RTComBenchmark)
