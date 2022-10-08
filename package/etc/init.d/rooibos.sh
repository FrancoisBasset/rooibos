#!/bin/sh

while [ true ]
do
	rooibos cache-update > /dev/null 2>&1

	if [ "$SUDO_USER" != "root" ]
	then
		ROOIBOS_USER=$SUDO_USER
	fi

	if [ "$USER" != "root" ]
	then
		ROOIBOS_USER=$USER
	fi

	chown -R $ROOIBOS_USER:$ROOIBOS_USER "/home/"$ROOIBOS_USER"/.rooibos"

	sleep 10
done
