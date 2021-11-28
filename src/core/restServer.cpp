#include"restServer.h"

#include"../utils/config.h"
#include"../console/log.h"
#include"../utils/exception.h"
#include"db.h"
#include"../utils/utils.h"

#include"openCL.h"

std::vector<std::string> RESTServer::urlToNodes(std::string &url){
    size_t postion=0;
    size_t postionTemp=0;
    bool run;
    char car=url[postion++];
    if(car!='/')throw Exception(0,"Error en el formato de la URL",this,INFO_LOG);
    std::vector<std::string> nodes;
    do{
        postionTemp=url.find_first_of('/',postion);
        std::string value=url.substr(postion,postionTemp-postion);
        Log::log(Log::trace,this,INFO_LOG,"Agregando nodo:",value);
        nodes.push_back(value);
        postion=postionTemp+1;
        run=postionTemp!=std::string::npos;
    }while(run);
    return nodes;
}

std::string RESTServer::documentToString(rapidjson::Document &document){
	std::string json="";
	if(!document.IsNull()){
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		document.Accept(writer);
		json=sb.GetString();
	}
	return json;
}

RESTServer::RESTServer(const std::vector<std::shared_ptr<RESTFunctionCommun> > &functions):
		functions(functions),
        AbstractServer(Config::getPort()){
	Log::log(Log::info,this,INFO_LOG,"Iniciando servidor REST");
}

Constant::nodes RESTServer::getEnumNode(const std::string &node){
	auto iterator=Constant::mapNodes.find(node);
	Constant::nodes enumNode=Constant::otherNode;
	if (iterator!=Constant::mapNodes.end()){
		enumNode=iterator->second;
	}
	return enumNode;
}

Constant::methods RESTServer::getEnumMethod(const std::string &method){
	auto iterator=Constant::mapMethods.find(method);
	Constant::methods enumMethod=Constant::otherMethod;
	if (iterator!=Constant::mapMethods.end()){
		enumMethod=iterator->second;
	}
	return enumMethod;
}

bool RESTServer::connection(AbstractServer::Response &response,AbstractServer::Request &request){
	Log::log(Log::debug,this,INFO_LOG,"Conexion establecida");
    response.headers["Content-Type"]="application/json";
    response.code=404;
    Constant::methods method=RESTServer::getEnumMethod(request.method);
    std::vector<std::string> nodes=this->urlToNodes(request.url);
    std::vector<Constant::nodes> nodesEnum;
    for(std::string &node:nodes){
    	nodesEnum.push_back(RESTServer::getEnumNode(node));
    }
    for(std::shared_ptr<RESTFunctionCommun> &function:functions){
    	if(function->filter(method, nodesEnum)){
    		//request
    		RESTFunctionCommun::Request requestREST;
    		requestREST.headers=&request.headers;
    		requestREST.nodes=&nodes;
    		requestREST.querys=&request.querys;
    		rapidjson::Document requestJson;
    		requestJson.Parse(request.body);
    		requestREST.json=&requestJson;
    		//response
    		RESTFunctionCommun::Response responseREST;
    		responseREST.headers=&response.headers;
    		rapidjson::Document responseJson;
    		responseREST.json=&responseJson;
    		response.code=function->operation(responseREST, requestREST);
    		response.body=RESTServer::documentToString(responseJson);
    	}
    }
    return true;
}

