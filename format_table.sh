#!/bin/bash

cat $1 | sed 's/<TR>/<tr>/g' |
sed 's/<\/TR>/<\/tr>/g' |
sed 's/<TD/<td/g' |
sed 's/<\/TD/<\/td/g' |
sed 's/<TH>/<th>/g' |
sed 's/<\/TH>/<\/th>/g' |
sed 's/<TABLE/<table/g' |
sed 's/<\/TABLE>/<\/table>/g' |
sed 's/align=\"right\"/class=\"right\"/g' |
sed 's/border=1/id=\"arrayexp\"/g' 
