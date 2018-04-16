# get header and check all functions

/^bool/ {
    gsub (/\(.*\);/, "", $2);
    gsub (/^check_/, "", $2);

    print "\tif (check_" $2 "())";
    print "\t\tprintf (\"" $2 "\\n\");";
}
