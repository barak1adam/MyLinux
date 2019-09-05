#!/bin/bash -x

#if down
#ifconfig vif100 down
ip link delete vif100


pid=$(cat /var/run/keepalived.pid)
echo $pid
cp /etc/keepalived/VI_vif100_1.conf /tmp
cp /etc/keepalived/VI_vif100_2.conf /tmp
cp /etc/keepalived/VI_vif100_3.conf /tmp
#cp /etc/keepalived/VI_vif101_11.conf /tmp

sed -i '/VI_vif100_1.conf/d' /etc/keepalived/keepalived.conf
sed -i '/VI_vif100_1.conf/d' /etc/keepalived/keepalived.conf
sed -i '/VI_vif100_1.conf/d' /etc/keepalived/keepalived.conf
#rm -f /etc/keepalived/VI_vif100_1.conf
cat /etc/keepalived/keepalived.conf


#reload empty file
kill -1 $pid


#if up
#ifconfig vif100 up
#ifconfig vif100 1.1.1.2 netmask 255.255.255.0 txqueuelen 500
vconfig add vif100 100
ip link add link vif100 name vif100.100 type vlan id 100
ip addr add 1.1.1.2/24 brd 1.1.1.255 dev vif100.100
ip link set dev vif100.100 up

#reload empty file
kill -1 $pid

#reload empty file
kill -1 $pid

#cp /tmp/VI_vif100_1.conf /etc/keepalived/VI_vif100_1.conf
echo "include /etc/keepalived/VI_vif100_1.conf" >> /etc/keepalived/keepalived.conf
cat /etc/keepalived/keepalived.conf

kill -1 $pid

echo "include /etc/keepalived/VI_vif100_2.conf" >> /etc/keepalived/keepalived.conf
cat /etc/keepalived/keepalived.conf

kill -1 $pid

echo "include /etc/keepalived/VI_vif100_3.conf" >> /etc/keepalived/keepalived.conf
cat /etc/keepalived/keepalived.conf


#sed -i '/VI_vif101_11.conf/d' /etc/keepalived/keepalived.conf
#rm -f /etc/keepalived/VI_vif101_11.conf
#cat /etc/keepalived/keepalived.conf
#cp /tmp/VI_vif101_11.conf /etc/keepalived/VI_vif101_11.conf 
#echo "include /etc/keepalived/VI_vif101_11.conf" >> /etc/keepalived/keepalived.conf
#cat /etc/keepalived/keepalived.conf

kill -1 $pid





copy-paste:
===========
sed -i '/VI_vif100_1.conf/d' /etc/keepalived/keepalived.conf
pid=$(cat /var/run/keepalived.pid)
kill -1 $pid
echo "include /etc/keepalived/VI_vif100_1.conf" >> /etc/keepalived/keepalived.conf
kill -1 $pid
sed -i '/VI_vif101_11.conf/d' /etc/keepalived/keepalived.conf
kill -1 $pid
echo "include /etc/keepalived/VI_vif101_11.conf" >> /etc/keepalived/keepalived.conf
kill -1 $pid
