#!/bin/bash
UIFILE=./ui_sel.h

sel_ui_basic()
{
tag1=`grep "$1" $UIFILE`
tag2=""
#if [[ ${tag1:0:1} -eq "#" ]] 
if [[ "$2" = "e" ]];then
tag2=$1
else
tag2="//"$1
fi
sed -i "s:$tag1:$tag2:" $UIFILE
}

sel_ui()
{
if [[ "$1" = "-t1" ]];then
	sel_ui_basic "#define TECHVISION_GUI" "e";
	sel_ui_basic "#define JUAN_NEW_UI" "d";
elif [[ "$1" = "-n1" ]];then
	sel_ui_basic "#define TECHVISION_GUI" "d";
	sel_ui_basic "#define JUAN_NEW_UI" "e";
elif [[ "$1" = "-o1" ]];then
	sel_ui_basic "#define TECHVISION_GUI" "d";
	sel_ui_basic "#define JUAN_NEW_UI" "d";
fi
echo $UIFILE":"
cat $UIFILE
echo ""
}

enable_techvision()
{
sel_ui "-t1"
cd gui
/bin/rm -rf resource
cp -a resource_techvision resource
cd luiengine/skin
rm controls.*
cp techvision/controls.* .
echo "All resources and skin files is ready for techvision ui now!"
cd ../../..
}

enable_juan_olduiskin1()
{
sel_ui "-o1"
cd gui
/bin/rm -rf resource
cp -a resource_def resource
cd luiengine/skin
rm controls.*
cp def/controls.* .
cd ../../..
echo "All resources and skin files is ready for default old ui now!"
}

enable_juan_newuiskin1()
{
sel_ui "-n1"
cd gui
/bin/rm -rf resource
cp -a resource_janewskin1 resource
cd luiengine/skin
rm controls.*
cp newskin1/controls.* .
cd ../../..
echo "All resources and skin files is ready for default new ui1 now!"
}


printusage()
{
echo "-o1 : enable juan old ui skin1";
echo "-n1 : enable juan new ui skin1";
echo "-t1 : enable techvision ui skin";
}

if [ $# = 0 ];then
	printusage;
elif [[ "$1" = "-t1" ]];then
	enable_techvision;
elif [[ "$1" = "-o1" ]];then
	enable_juan_olduiskin1;
elif [[ "$1" = "-n1" ]];then
	enable_juan_newuiskin1;
else
	printusage;
fi
