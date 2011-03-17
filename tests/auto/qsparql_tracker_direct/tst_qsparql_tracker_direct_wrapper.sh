#!/bin/bash

tracker-import testdata_tracker_direct.ttl

if [ $? -ne 0 ] ; then
        echo "Could not import test data"
        exit $?
fi

numberOfContacts=2000

query=`./massinsert.sh $numberOfContacts`
# echo $query
tracker-sparql -u --query "$query"

./tst_qsparql_tracker_direct
er=$?

tracker-sparql -u -f clean_data_tracker_direct.rq

i=1
while [ $i -le $numberOfContacts ]
do
    query=`./deletecontact.sh $i`
#     echo $query
    tracker-sparql -u --query "$query"
    i=$(($i+1))
done

exit $er
