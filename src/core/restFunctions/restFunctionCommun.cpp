#include "restFunctionCommun.h"

RESTFunctionCommun::RESTFunctionCommun(const std::string &method,const std::string &path):
		method(method),
		path(path),
		restServer(NULL){

}

int RESTFunctionCommun::operation(Response &response,const Request &request) {
	return 404;
}

void RESTFunctionCommun::init(RESTServer &restServer){
	std::unique_lock writeLock(this->mutex);
	this->restServer=&restServer;
}

void RESTFunctionCommun::end(){
	std::unique_lock writeLock(this->mutex);
	this->restServer=NULL;
}

void RESTFunctionCommun::createMessageError(rapidjson::Document &document,const std::string &error){
    auto &allocator=document.GetAllocator();
    document.SetObject();
    document.AddMember("error",error,allocator);
}

bool RESTFunctionCommun::filter(Constant::methods method,const std::vector<Constant::nodes> &nodes) {
	return false;
}

int RESTFunctionCommun::operationAndTelemetry(Response &response,const Request &request){
	std::shared_lock readLock(this->mutex);
	if(!this->restServer){
		throw Exception(0,"Servidor no encontrado",INFO_LOG);
	}
	count++;
	return this->operation(response,request);
}

unsigned int RESTFunctionCommun::getAndResetCount(){
	return this->count.exchange(0);
}

const std::string& RESTFunctionCommun::getMethod(){
	return this->method;
}

const std::string& RESTFunctionCommun::getPath(){
	return this->path;
}
