#########################################################################
# File Name: bmp2yuv.sh
# Author: kaga
# mail: kejiazhw@163.com
# Created Time: 2015年04月16日 星期四 17时40分50秒
#########################################################################
#!/bin/bash

make_chn_skin()
{
CURDIR=`pwd`
cd app/skin;
make
echo "CURDIR:"$CURDIR
find -maxdepth 1 -name "*.yuv"|xargs /bin/rm -f
find -maxdepth 1 -name "*.bmp"|xargs /bin/rm -f
find -maxdepth 1 -name "*.h264"|xargs /bin/rm -f
if [[ "$1" = "-d1" ]];then
	cp -f dvr_def/*.bmp .;
elif [[ "$1" = "-n1" ]];then
	cp -f nvr_def/*.bmp .;
elif [[ "$1" = "-h1" ]];then
	cp -f hvr_def/*.bmp .;
	cp -f hvr_def/*.h264 .;
else
	echo $0" ]] invalid paramteter:"$1
	cd -
	exit 1;
fi
for fname in `find -maxdepth 1 -type f -name "*.bmp"| cut -d/ -f 2|cut -d. -f 1` ;do echo "create channel skin:" $fname; ./bmp2yuv $fname;done
find -maxdepth 1 -name "*.bmp"|xargs /bin/rm -f
cd -;
}


printusage()
{
echo "-d1 : enable dvr channel skin1";
echo "-n1 : enable nvr channel skin1";
echo "-h1 : enable hvr channel skin1";
}

if [ $# = 0 ];then
	printusage;
else
	make_chn_skin $1
fi
