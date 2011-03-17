#!/bin/sh

usage()
{
    echo "usage: deletecontact.sh sequenceNumber";
}

case $# in
    0)
	usage;
	exit 1;
	;;
esac

condition="?u a nco:PersonContact ; nco:nameGiven \"Given$1\" ; nco:nameFamily \"Family$1\" ; nco:hasPhoneNumber ?pn . ?pn a nco:PhoneNumber ; nco:phoneNumber \"phone$1\" ."
query="delete { $condition } where { $condition }";

echo $query

# tracker-sparql -qu "$query"
