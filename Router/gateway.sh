ifconfig | perl -nle 's/dr:(\S+)/print $1/e' | head -1
