
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

function loadfinish()
{
	xmlHttp = createXMLHttpRequest();  
	var url = document.getElementById('contentbuffer').innerText;
	xmlHttp.open("GET", url, true);
	xmlHttp.onreadystatechange = function()
	{
		if(xmlHttp.readyState === 4 && xmlHttp.status === 200)
		{
			Preview.markdown = xmlHttp.responseText;
			console.log(Preview.markdown);
			Preview.Update();
		}   
	}

	xmlHttp.setRequestHeader("Content-Type",  
			        "application/x-www-form-urlencoded;");  

	xmlHttp.send(); 
}

