#include "restFunctionCommun.h"

void RESTFunctionCommun::createMessageError(rapidjson::Document &document,const std::string &error){
    auto &allocator=document.GetAllocator();
    document.SetObject();
    document.AddMember("error",error,allocator);
}

bool RESTFunctionCommun::filter(Constant::methods method,const std::vector<Constant::nodes> &nodes) {
	return false;
}

int RESTFunctionCommun::operation(Response &response,const Request &request) {
	return 404;
}
