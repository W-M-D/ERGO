#!/bin/sh

mv /boot/cmdline.txt /boot/cmdline.txt.bak
sed  's/\S*\ttyAMA0\\S*//g' /boot/cmdline.txt.bak > /boot/cmdline.txt

mv /etc/inittab /etc/inittab.bak
sed '/ttyAMA0/d' /etc/inittab.bak > /etc/inittab



