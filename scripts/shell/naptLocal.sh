#!/bin/bash

iptables -F
iptables -F -t nat

iptables -t nat -A POSTROUTING -s 192.168.3.0/24 -d 192.168.50.0/24 -j MASQUERADE
iptables -t nat -A POSTROUTING -s 192.168.50.0/24 -o enp0s3 -j MASQUERADE

iptables -F