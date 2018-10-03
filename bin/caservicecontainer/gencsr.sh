#!/bin/bash
thread_count=$1
openssl req -nodes -sha1 -newkey rsa:2048 -keyout Thread$thread_count.key -out Thread$thread_count.csr -days 365 -subj  "/C=US/ST=CA/L=SantaClara/O=ONAP/CN=<name>"
chmod +x *
