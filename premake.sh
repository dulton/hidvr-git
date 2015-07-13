#########################################################################
# File Name: premake.sh
# Author: kaga
# mail: kejiazhw@163.com
# Created Time: 2015年03月26日 星期四 10时27分55秒
#########################################################################
#!/bin/bash
PRJ_PATH=`grep "PRJ_PATH  :=" Makefile.param | awk '{print $3}'`
CURDIR=`pwd`
echo "PRJ_PATH:"$PRJ_PATH
echo "CURDIR:"$CURDIR
sed -i 's:'$PRJ_PATH':'$CURDIR':' ./Makefile.param
PRJ_PATH=`grep "PRJ_PATH  :=" Makefile.param | awk '{print $3}'`
echo "After ->PRJ_PATH:"$PRJ_PATH

sed -i 's:VER=`git:VER=\"\"; #`git:' svn2conf
VER=`grep "git svn info" svn2conf | awk -F= '{print $2}'`
echo "After ->VER:"$VER
