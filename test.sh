echo "Starting Testing"

echo "--Sending good request with space delimeters--"
echo -e -n "GET / HTTP/1.0\r\n\r\n" |  nc -u -q 1 -s 192.168.1.100 10.10.1.100 8080
echo "--Sending good request with lower case letters--"
echo -e -n "Get / HttP/1.0\r\n\r\n" |  nc -u -q 1 -s 192.168.1.100 10.10.1.100 8080
echo "--Sending good request with tab delimeters for index.html\n--"
echo -e -n "GET\t/\tHTTP/1.0\r\n\r\n" |  nc -u -q 1 -s 192.168.1.100 10.10.1.100 8080
echo "--Sending good request with tab delimeters  for text.txt\n--"
echo -e -n "GET\t/text.txt\tHTTP/1.0\r\n\r\n" |  nc -u -q 1 -s 192.168.1.100 10.10.1.100 8080
echo "--Sending good request with multiple / combo space tab delimeters\n--"
echo -e -n "GET\t \t  /   \t\tHTTP/1.0\r\n\r\n" |  nc -u -q 1 -s 192.168.1.100 10.10.1.100 8080
echo "--Sending bad request with new lines in between things\n--"
echo -e -n "GET\t \r\n\t  /   \t\n\n\r\tHTTP/1.0\r\n\r\n" |  nc -u -q 1 -s 192.168.1.100 10.10.1.100 8080
echo "--Sending bad request navigating up directory structure\n--"
echo -e -n "GET\t\t  /../../text.txt   \t\tHTTP/1.0\r\n\r\n" |  nc -u -q 1 -s 192.168.1.100 10.10.1.100 8080
echo "--Sending bad request with unsupported version string\n--"
echo -e -n "GET / HTTP/1.1\r\n\r\n" |  nc -u -q 1 -s 192.168.1.100 10.10.1.100 8080
echo "--Sending bad request with headers not ending in a blank line\n--"
echo -e -n "GET / HTTP/1.1 asdas\n asdas\n asdasd\r\n" |  nc -u -q 1 -s 192.168.1.100 10.10.1.100 8080