#include "callback.h"
#include "basetool.h"
#include "enums.h"
#include "neter.h"

using namespace TCORE;

void HttpCallbackTask::Exec(){
	HttpCallback callback;
	HttpPacketVisitor<HttpRequest> reqvis(req);
	if(CallbackManager<HttpCallback>::GetInstance().Get(reqvis.GetURL(), callback) == false){
		callback = [](const HttpRequest& )->const HttpResponse {
			HttpResponse res;
			HttpPacketVisitor<HttpResponse> vis(res);
			vis.SetVersion("HTTP/1.1");
			vis.SetStatus(HTTP_SC_NOT_FOUND);
			vis.SetStatement(GetStatusCodeInfo(HTTP_SC_NOT_FOUND));
			vis.SetHeader("Content-Length", tostring(9));
			vis.SetBody("NOT FOUND");
			return res;
		};
	}
	Neter::SendProtocol(sid, callback(req));
}
