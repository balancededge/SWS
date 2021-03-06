# Test cases

# Test Valid request
echo -e -n "GET /index.html HTTP/1.0\r\n\r\n" | nc -u -s 192.168.1.100 10.10.1.100 8000 &
# Test assumed index.html
echo -e -n "GET / HTTP/1.0\r\n\r\n" | nc -u -s 192.168.1.100 10.10.1.100 8000 &
# Test large file
echo -e -n "GET /gnu/main.html HTTP/1.0\r\n\r\n" | nc -u -s 192.168.1.100 10.10.1.100 8000 &
# Test bad request
echo -e -n "GET / HTTP/1.1\r\n\r\n" | nc -u -s 192.168.1.100 10.10.1.100 8000 &
# Test not found
echo -e -n "GET /nofile HTTP/1.0\r\n\r\n" | nc -u -s 192.168.1.100 10.10.1.100 8000 &
# Test out of servered directory
echo -e -n "GET /../makefile HTTP/1.0\r\n\r\n" | nc -u -s 192.168.1.100 10.10.1.100 8000 &