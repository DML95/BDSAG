#include "countSessions.h"

#include"../db.h"
#include"../../utils/utils.h"
#include"../../utils/config.h"

CountSessions::CountSessions():
	RESTFunctionCommun("GET", "/database/sessions/count"){

}

bool CountSessions::filter(Constant::methods method,const std::vector<Constant::nodes> &nodes) {
	if(method!=Constant::get)return false;
	if(nodes.size()!=3)return false;
	if(nodes[2]!=Constant::count)return false;
	if(nodes[1]!=Constant::sessions)return false;
	if(nodes[0]!=Constant::database)return false;
	return true;
}

int CountSessions::operation(Response &response,const Request &request) {
	Log::log(Log::debug,this,INFO_LOG,"GET /database/sessions/count");
	auto &allocator=response.json->GetAllocator();
	response.json->SetObject();
	response.json->AddMember("count",DB::countSessions(),allocator);
	response.json->AddMember("maxcount",Config::getSizeDB(),allocator);
	return 200;
}
