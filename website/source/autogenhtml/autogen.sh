#!/bin/bash

IFS="
"

record_dir="../record/"
html_dir="./"

record_name_set=()
template_line_set=()

function GenHTML()
{
	record_name=$1
	html_name=`echo ${record_name} | awk -F '/' '{print $NF}' | awk -F '.' '{printf $1}'`".html"
	echo ${html_name}

	echo "" > ${html_name}

	for template_line in ${template_line_set[*]}
	do
		res=`echo ${template_line} | grep "I need something"`;
		if [ ${res} ]
		then
			echo "/record/"`echo ${record_name} | awk -F "${record_dir}" '{print $2}'` >> ${html_name}
		else
			echo ${template_line} >> ${html_name}
		fi
	done
}

function InitTemplateFile()
{
	template_file=$1
	for line in `cat ${template_file}`
	do
		template_line_set[${#template_line_set[*]}]=${line}
	done
}

InitTemplateFile "./template"

for record_name in `find ${record_dir} -name "*"`
do
	res=`echo ${record_name} | grep -E "md$"`
	if [ ${res} ]; then
		record_name_set[${#record_name_set[*]}]=${record_name}
	fi
done

for record_name in ${record_name_set[*]}
do
	GenHTML ${record_name}
done
