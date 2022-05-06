#!/bin/bash

# Initialize the lease file if it doesn't exist.
#touch /data/dhcpd/dhcpd.leases


# Iniciar el servidor tcp de C
calculadora_subred/make server -B

#/etc/init.d/isc-dhcp-server start
#dhcpd -cf /etc/dhcp/dhcpd.conf -lf /var/lib/dhcp/dhcpd.leases --no-pid -4 -f
#tail -f /dev/null


#chmod 660 /etc/bind/rndc.conf
#chown root:bind /etc/bind/rndc.conf
#cd /etc/dhcp
#ln -s /etc/bind/rndc.conf

