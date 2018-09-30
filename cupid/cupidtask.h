#include "threadpool.h"
#include "cupidbase.h"
#include "basetool.h"
#include "protocol.h"
#include "neter.h"
#include "enums.h"
#include "file.h"

namespace TCORE
{

class HandleHttpRequestTask : public Task
{
public:
	HandleHttpRequestTask(task_id_t task_id, session_id_t s, const HttpRequest &req)
	: Task(task_id)
	, sid(s)
	, request(req)
	{}

	void Exec()
	{
		HttpResponse res;

		HttpPacketVisitor<HttpResponse> res_vis(res);
		HttpPacketVisitor<HttpRequest>  req_vis(request);

		FileManager::FilePtr file = FileManager::GetInstance().GetFilePtr(default_base_folder+req_vis.GetURL());

		Log::Trace("HandleHttpRequestTask::Exec, url=", req_vis.GetURL());

		if(file)
		{
			res_vis.SetVersion("HTTP/1.1");
			res_vis.SetStatus(HTTP_SC_OK);
			res_vis.SetStatement(GetStatusCodeInfo(HTTP_SC_OK));
			res_vis.SetBody(file->Data());
			res_vis.SetHeader("Content-Type", GetMimeType(GetFileSuffixName(req_vis.GetURL())));
			res_vis.SetHeader("Content-Length", tostring(file->Size()));
		}
		else
		{
			std::string tmp_name = default_base_folder + req_vis.GetURL() + default_file_name;
			file = FileManager::GetInstance().GetFilePtr(tmp_name);
			if(file)
			{
				res_vis.SetVersion("HTTP/1.1");
				res_vis.SetStatus(HTTP_SC_OK);
				res_vis.SetStatement(GetStatusCodeInfo(HTTP_SC_OK));
				res_vis.SetBody(file->Data());
				res_vis.SetHeader("Content-Type", GetMimeType(GetFileSuffixName(tmp_name)));
				res_vis.SetHeader("Content-Length", tostring(file->Size()));
			}
			else
			{
				res_vis.SetVersion("HTTP/1.1");
				res_vis.SetStatus(HTTP_SC_NOT_FOUND);
				res_vis.SetStatement(GetStatusCodeInfo(HTTP_SC_NOT_FOUND));
				res_vis.SetHeader("Content-Length", tostring(9));
				res_vis.SetBody("NOT FOUND");
			}
		}

		Neter::SendProtocol(sid, res);
	}

private:
	session_id_t sid;
	HttpRequest request;
};

}
