
function createXMLHttpRequest()
{  
	var xmlHttp;  
	if (window.XMLHttpRequest) {  
		xmlHttp = new XMLHttpRequest();  
		if (xmlHttp.overrideMimeType)  
			xmlHttp.overrideMimeType('text/xml');  
	} else if (window.ActiveXObject) {  
		try {  
			xmlHttp = new ActiveXObject("Msxml2.XMLHTTP");  
		} catch (e) {  
			try {  
				xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");  
			} catch (e) {  
			}  
		}  
	}  
	return xmlHttp;  
}

function is_empty_line(line)
{
	return line == "";
	//return /^\s*$/.test(line);
}

function is_code_line(line)
{
	return /^\s/.test(line);
}

function is_quote_line(line)
{
	return /^>\s/.test(line);
}

function is_list_line(line)
{
	return /^\s*([\+\-\*]|[0-9]+\.) /.test(line);
}

function is_table_line(line)
{
	return /\|\s[^\s]/.test(line);
}

function ParseLine(line)
{
	var tmp = "";
	for(var i = 0; i < line.length; ++i)
	{
		if(line[i] == '&')
		{
			tmp += '&amp';
		}
		else if(line[i] == '>')
		{
			tmp += '&gt;';
		}
		else if(line[i] == '<')
		{
			tmp += '&lt;';
		}
		else if(line[i] == ' ')
		{
			tmp += '&nbsp;';
		}
		else if(line[i] == '"')
		{
			tmp += '&quot;';
		}
		else
		{
			tmp += line[i];
		}
	}

	if(/^#+&nbsp;/.test(tmp))
	{
		var i = 0;
		for(; i < tmp.length && tmp[i] == '#'; ++i)
		{
		}
		i = (i <= 6 ? i : 6);
		tmp = tmp.replace(/^#+&nbsp;/, "<h"+ i + ">");
		tmp += "</h" + i + ">";
	}


	return tmp + "<br>";
}

var parse_list_lines_new_begin_line;
function ParseListLines(substring, begin, end, pre_tab_count)
{
	var html = "";
	const NONE = 0;
	const ORDER = 1;
	const UNORDER =2;

	var CountTab = function(str){
		for(var i = 0; i < str.length; ++i)
		{
			if(str[i] != '\t')
			{
				return i;
			}
		}
	};

	var sta = NONE;
	var cur_pre_tab_count = pre_tab_count;
	while(begin < end)
	{
		var now_tab_count = CountTab(substring[begin]);
		if(now_tab_count == cur_pre_tab_count)
		{
			html += "<li>" + ParseLine(substring[begin].substring(now_tab_count + 2)) + "</li>";
		}
		else
		{
			if(sta == NONE)
			{
				cur_pre_tab_count = now_tab_count;
				if(/\s*[\*\+\-] /.test(substring[begin]))
				{
					sta = UNORDER;
					html += "<ul>";
					html += "<li>" + ParseLine(substring[begin].substring(now_tab_count + 2)) + "</li>";
				}
				else
				{
					sta = ORDER;
					html += "<ol>";
					html += "<li>" + ParseLine(substring[begin].substring(now_tab_count + 2)) + "</li>";
				}
			}
			else
			{
				if(cur_pre_tab_count < now_tab_count)
				{
					html += ParseListLines(substring, begin, end);
					begin = parse_list_lines_new_begin_line - 1;
				}
				else
				{
					parse_list_lines_new_begin_line = begin;
					html += (sta == UNORDER ? "</ul>" : "</ol>");
					return html;
				}
			}
		}
		++begin;
	}
	if(sta == ORDER) html += "</ol>";
	else if(sta == UNORDER) html += "</ul>";
	return html;
}

function ParseTableLines(substring, begin, end)
{
	return "";
}

function ParseSectionLine(str)
{
	return ParseLine(str);
}

function SimpleMarkdownInterpreter(markdown)
{
	var substring = markdown.split("\n");	
	substring.push("");
	var html = "";

	// ---------------------------- //
	
	const empty_status = 0;
	const quote_status = 1;
	const code_status = 2;
	const section_status = 3;
	const list_status = 4;
	const table_status = 5;
	
	var cur_parser_status = empty_status;

	var code_begin_line_num = -1;
	var quote_begin_line_num = -1;
	var list_begin_line_num = -1;
	var table_begin_line_num = -1;

	for(var i = 0; i < substring.length; ++i)
	{
		var cur_line = substring[i];
		if(cur_parser_status == empty_status)
		{
			if(is_empty_line(cur_line))
			{
				continue;
			}

			if(is_code_line(cur_line))
			{
				code_begin_line_num = i;
				cur_parser_status = code_status;
				continue;
			}

			if(is_quote_line(cur_line))
			{
				quote_begin_line_num = i;
				cur_parser_status = quote_status;
				continue;
			}

			if(is_list_line(cur_line))
			{
				list_begin_line_num = i;
				cur_parser_status = list_status;
				continue;
			}

			if(is_table_line(cur_line))
			{
				table_begin_line_num = i;
				cur_parser_status = table_status;
				continue;
			}
			cur_parser_status = section_status;
		}
		else if(cur_parser_status == code_status)
		{
			var tail = false;
			do
			{
				if(is_empty_line(cur_line))
				{
					tail = true;
					cur_parser_status = empty_status;
					break;
				}

				if(is_code_line(cur_line))
				{
					break;
				}

				cur_parser_status = section_status;
				tail = true;
				break;
			}while(0);

			if(tail == true)
			{
				html += '<div class="code_border">';
				while(code_begin_line_num < i)
				{
					html += '<code>' + ParseLine(substring[code_begin_line_num]) + '</code>';
					++code_begin_line_num;
				}

				html += '</div>'
			}
		}
		else if(cur_parser_status == quote_status)
		{
			var tail = false;
			do
			{
				if(is_empty_line(cur_line))
				{
					tail = true;
					cur_parser_status = empty_status;
					break;
				}

				if(is_quote_line(cur_line))
				{
					break;
				}

				cur_parser_status = seciton_status;
				tail = true;
				break;
			}while(0);

			if(tail == true)
			{
				html += '<div class="quote_border">';
				while(quote_begin_line_num < i)
				{
					html += ParseLine(substring[quote_begin_line_num].substring(2));
					++quote_begin_line_num;
				}

				html += '</div>'
			}
		}
		else if(cur_parser_status == list_status)
		{
			var tail = false;
			do
			{
				if(is_empty_line(cur_line))
				{
					tail = true;
					cur_parser_status = empty_status;
					break;
				}

				if(is_list_line(cur_line))
				{
					break;
				}

				cur_parser_status = section_status;
				tail = true;
				break;
			}while(0);

			if(tail == true)
			{
				parse_list_lines_new_begin_line = 0;
				html += ParseListLines(substring, list_begin_line_num, i, -1, parse_list_lines_new_begin_line);
			}
		}
		else if(cur_parser_status == table_status)
		{
			var tail = false;
			do
			{
				if(is_empty_line(cur_line))
				{
					tail = true;
					cur_parser_status = empty_status;
					break;
				}

				if(is_table_line(cur_line))
				{
					break;
				}

				cur_parser_status = seciton_status;
				tail = true;
				break;
			}while(0);

			if(tail == true)
			{
				html += ParseTableLines(substring, table_begin_line_num, i);
			}
		}

		if(cur_parser_status == section_status)
		{
			if(is_empty_line(cur_line))
			{
				cur_parser_status = empty_status;
				continue;
			}
			html += ParseSectionLine(cur_line);
		}
	}

	return html;
}

function loadfinish()
{
	xmlHttp = createXMLHttpRequest();  
	var url = document.getElementById('content').innerText;
	xmlHttp.open("GET", url, true);
	xmlHttp.onreadystatechange = function()
	{
		if(xmlHttp.readyState === 4 && xmlHttp.status === 200)
		{
			document.getElementById('content').innerHTML = SimpleMarkdownInterpreter(xmlHttp.responseText);
		}   
	}
	xmlHttp.setRequestHeader("Content-Type",  
			        "application/x-www-form-urlencoded;");  
	xmlHttp.send(); 
}

