#!/bin/sh

usage()
{
    echo "usage: massinsert.sh howMany";
}

case $# in
    0)
	usage;
	exit 1;
	;;
esac

query="insert { ";

i=1
while [ $i -le $1 ]
do
    query="$query _:u$i a nco:PersonContact ; nco:nameGiven \"Given$i\" ; nco:nameFamily \"Family$i\" ; nco:hasPhoneNumber _:pn$i . _:pn$i a nco:PhoneNumber ; nco:phoneNumber \"phone$i\" ."
    i=$(($i+1))
done

query="$query }"

echo $query

# tracker-sparql -qu "$query"
