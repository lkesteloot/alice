
.PHONY: all install enable start stop

all:
	@echo See Makefile for targets.

install:
	sudo install --owner=root --group=root --mode=644 *.service /etc/systemd/system

enable:
	sudo systemctl enable alice4_start
	sudo systemctl enable alice4_shutdown

start:
	sudo service alice4_start start

stop:
	sudo service alice4_shutdown start

