#include "cupid.h"
#include "callback.h"
#include "enums.h"
#include "basetool.h"
#include <iostream>
#include <string>
#include <pcrecpp.h>

using namespace std;

void TCORE::ImportModule(){
	{
		int i;
		string s;
		pcrecpp::RE re("(\\w+):(\\d+)");
		if (re.error().length() > 0) {
			cout << "PCRE compilation failed with error: " << re.error() << "\n";
		}
		if (re.PartialMatch("root:1234", &s, &i))
			cout << s << " : " << i << "\n";
	}
	HttpCallback example = [](const HttpRequest &r)->const HttpResponse{
		HttpResponse res;
		HttpPacketVisitor<HttpResponse> vis(res);
		vis.SetVersion("HTTP/1.1");
		vis.SetStatus(HTTP_SC_OK);
		vis.SetStatement(GetStatusCodeInfo(HTTP_SC_OK));
		const std::string body = "Hello World !";
		vis.SetHeader("Content-Length", tostring(body.size()));
		vis.SetBody(body);
		return res;
	};
	CallbackManager<HttpCallback>::GetInstance().Set("/video", example);
}
