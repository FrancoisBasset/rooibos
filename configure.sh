green="\e[32m"
red="\e[31m"
normal="\e[0m"

check_stdlib() {
	echo "Checking stdlib installed..."

	if [ -f /usr/include/stdlib.h ]
	then
		echo -e "${green}stdlib is installed $normal"
	else
		echo -e "${red}Please install build-essential $normal"
	fi
}

check_installed() {
	echo "Checking $1 installed..."

	dpkg -l $1 > /dev/null 2>&1

	if [ $? -eq 0 ]
	then
		echo -e "$green$1 is installed $normal"
	else
		echo -e "${red}Please install $1 $normal"
	fi
}

check_stdlib
check_installed libsqlite3-dev
