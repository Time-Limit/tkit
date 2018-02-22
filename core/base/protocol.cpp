#include "protocol.h"
#include "enums.h"
#include "basetool.h"
#include "session.h"

void HttpProtocol::Handle(SessionManager *manager, session_id_t sid)
{
	HttpResponse response;
	response.version = "HTTP/1.1";
	ResetHttpResponseStatus(response, HTTP_SC_OK);

	response.body =
	"<html>"
	"<h1 align=\"center\">" + tostring(response.status) + " " + response.statement + "</h1>"
	"<hr></hr>"
	"<p align=\"center\">tcore</p>"
	"</html>";
	ForceSetHeader(response, HTTP_CONTENT_LENGTH, tostring(response.body.size()));
	ForceSetHeader(response, HTTP_CONTENT_TYPE, GetMimeType("html"));

	std::stringstream ss;
	ss << response;

	manager->Send(sid, ss.str().c_str(), ss.str().size());
}
