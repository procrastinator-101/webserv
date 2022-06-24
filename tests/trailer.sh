#!/bin/bash

echo -en "POST http://localhost:9999/ HTTP/1.1\r\nUser-Agent: curl/7.35.0\r\nHost: localhost:9999\r\nAccept: */*\r\nTransfer-Encoding: chunked\r\nTrailer: HEA\r\n\r\n3\r\nxyz\r\n0\r\nHEA: xxx\r\n\r\n" | nc localhost 8080