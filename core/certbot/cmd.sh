#!/bin/sh
./certbot certonly --email tlimit@outlook.com --agree-tos --no-eff-email --webroot -w ./cert -d www.tlimit.cn
