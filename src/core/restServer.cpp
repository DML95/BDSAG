#include"restServer.h"

#include"../utils/config.h"
#include"../console/log.h"
#include"../utils/exception.h"
#include"db.h"
#include"../utils/utils.h"

#include"openCL.h"

std::unordered_map<std::string,RESTServer::enumNodes> RESTServer::mapNodes={
	{"database",RESTServer::database},
	{"session",RESTServer::session},
	{"config",RESTServer::config},
	{"devices",RESTServer::devices},
	{"sessions",RESTServer::sessions},
	{"count",RESTServer::count},
};

std::unordered_map<std::string,RESTServer::enumMethods> RESTServer::mapMethods={
	{"GET",RESTServer::get},
	{"POST",RESTServer::post},
	{"PATCH",RESTServer::patch},
	{"DELETE",RESTServer::deleete},
};

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

void RESTServer::createMessageError(rapidjson::Document &document,std::string error){
    auto &allocator=document.GetAllocator();
    document.SetObject();
    document.AddMember("error",error,allocator);
}

std::string RESTServer::databaseConfigGET(){
	Log::log(Log::debug,this,INFO_LOG,"GET /database/config");
	rapidjson::Document response;
	auto &allocator=response.GetAllocator();
	response.SetObject();
	response.AddMember("sizeDB",Config::getSizeDB(),allocator);
	response.AddMember("sizeSesion",Config::getSizeSesion(),allocator);
	response.AddMember("port",Config::getPort(),allocator);
	response.AddMember("logs",Config::getLogs(),allocator);
	return RESTServer::documentToString(response);
}

std::string RESTServer::databaseDevicesGET(){
	Log::log(Log::debug,this,INFO_LOG,"GET /database/devices");
	rapidjson::Document response;
	auto &allocator=response.GetAllocator();
	response.SetObject();
	rapidjson::Document devices;
	devices.SetArray();
	for(OpenCL &deviceCL:OpenCL::getDevices()){
		rapidjson::Value device;
		device.SetObject();
		device.AddMember("platform",deviceCL.getPlatform(), allocator);
		device.AddMember("name",deviceCL.getName(), allocator);
		device.AddMember("type",deviceCL.getType(), allocator);
		device.AddMember("reservedsessions",deviceCL.getSizeBufers(), allocator);
		devices.PushBack(device, allocator);
	}
	response.AddMember("devices", devices, allocator);
	return RESTServer::documentToString(response);
}

int RESTServer::databaseSessionPOST(rapidjson::Document &response,rapidjson::Document &request,std::unordered_map<std::string,std::string> &headers){
	Log::log(Log::debug,this,INFO_LOG,"POST /database/session");
    std::unordered_map<std::string,std::string>::iterator headerIterator=headers.find("User-Agent");
    if(headerIterator==headers.end()){
    	Log::log(Log::warn,this,INFO_LOG,"User-Agent no encontrado");
        RESTServer::createMessageError(response,"header: User-Agent");
        return 400;
    }
    DB::data data;
    data.userAgent=headerIterator->second;
    if(!request.IsObject()){
    	Log::log(Log::warn,this,INFO_LOG,"JSON invalido");
        RESTServer::createMessageError(response,"JSON");
        return 400;
    }
    rapidjson::Value value;
    if(!Utils::isMember(value,request,"expiretime")||!value.IsInt64()||(data.expireTime=value.GetInt64())<=0){
    	Log::log(Log::warn,this,INFO_LOG,"expiretime invalido");
        RESTServer::createMessageError(response,"JSON: expiretime");
        return 400;
    }
    if(!Utils::isMember(value,request,"value")||!value.IsString()){
    	Log::log(Log::warn,this,INFO_LOG,"value invalido");
        RESTServer::createMessageError(response,"JSON: value");
        return 400;
    }
    data.value=value.GetString();
    switch(DB::createSession(data)){
        case DB::dbFull:
        	Log::log(Log::warn,this,INFO_LOG,"base de datos llena");
            return 507;
    }
    auto &allocator=response.GetAllocator();
    response.SetObject();
    response.AddMember("id",data.session,allocator);
    response.AddMember("expireepoch",data.expireTime,allocator);
    return 201;
}

int RESTServer::databaseSessionGET(rapidjson::Document &response,std::string &id,std::unordered_map<std::string,std::string> &headers){
	Log::log(Log::debug,this,INFO_LOG,"GET /database/session/{id}");
    std::unordered_map<std::string,std::string>::iterator headerIterator=headers.find("User-Agent");
    if(headerIterator==headers.end()){
    	Log::log(Log::warn,this,INFO_LOG,"User-Agent no encontrado");
        RESTServer::createMessageError(response,"header: User-Agent");
        return 400;
    }
    DB::data data;
    data.userAgent=headerIterator->second;
    data.session=id;
    switch(DB::getSession(data)){
        case DB::sessionNotFound:
        	Log::log(Log::warn,this,INFO_LOG,"sesion no encontrada");
            return 404;
    }
    auto &allocator=response.GetAllocator();
    response.SetObject();
    response.AddMember("value",data.value,allocator);
    response.AddMember("expireepoch",data.expireTime,allocator);
    return 200;
}

int RESTServer::databaseSessionPATCH(rapidjson::Document &response,rapidjson::Document &request,std::string &id,std::unordered_map<std::string,std::string> &headers){
	Log::log(Log::debug,this,INFO_LOG,"PATCH /database/session");
    std::unordered_map<std::string,std::string>::iterator headerIterator=headers.find("User-Agent");
    if(headerIterator==headers.end()){
    	Log::log(Log::warn,this,INFO_LOG,"User-Agent no encontrado");
        RESTServer::createMessageError(response,"header: User-Agent");
        return 400;
    }
    DB::data data;
    data.userAgent=headerIterator->second;
    if(!request.IsObject()){
    	Log::log(Log::warn,this,INFO_LOG,"JSON invalido");
        RESTServer::createMessageError(response,"JSON");
        return 400;
    }
    rapidjson::Value value;
    data.updateExpireTime=Utils::isMember(value,request,"expiretime");
    if(data.updateExpireTime){
        if(!value.IsInt64()||(data.expireTime=value.GetInt64())<=0){
        	Log::log(Log::warn,this,INFO_LOG,"expiretime invalido");
            RESTServer::createMessageError(response,"JSON: expiretime");
            return 400;
        }
    }
    data.updateValue=Utils::isMember(value,request,"value");
    if(data.updateValue){
        if(!value.IsString()){
        	Log::log(Log::warn,this,INFO_LOG,"value invalido");
            RESTServer::createMessageError(response,"JSON: value");
            return 400;
        }
        data.value=value.GetString();
    }
    data.session=id;
    switch(DB::patchSession(data)){
        case DB::sessionNotFound:
        	Log::log(Log::warn,this,INFO_LOG,"sesion no encontrada");
            return 404;
    }
    auto &allocator=response.GetAllocator();
    response.SetObject();
    response.AddMember("value",data.value,allocator);
    response.AddMember("expireepoch",data.expireTime,allocator);
    return 200;
}

int RESTServer::databaseSessionDELETE(rapidjson::Document &response,std::string &id,std::unordered_map<std::string,std::string> &headers){
	Log::log(Log::debug,this,INFO_LOG,"DELETE /database/session/{id}");
    std::unordered_map<std::string,std::string>::iterator headerIterator=headers.find("User-Agent");
    if(headerIterator==headers.end()){
    	Log::log(Log::warn,this,INFO_LOG,"User-Agent no encontrado");
        RESTServer::createMessageError(response,"header: User-Agent");
        return 400;
    }
    DB::data data;
    data.userAgent=headerIterator->second;
    data.session=id;
    switch(DB::deleteSession(data)){
        case DB::sessionNotFound:
        	Log::log(Log::warn,this,INFO_LOG,"sesion no encontrada");
            return 404;
    }
    auto &allocator=response.GetAllocator();
    response.SetObject();
    response.AddMember("value",data.value,allocator);
    response.AddMember("expireepoch",data.expireTime,allocator);
    return 200;
}

int RESTServer::databaseSessionsGET(rapidjson::Document &response,std::unordered_map<std::string,std::string> &querys){
	std::unordered_map<std::string,std::string>::iterator queryIterator=querys.find("value");
	if(queryIterator==querys.end()){
		Log::log(Log::warn,this,INFO_LOG,"value no encontrado");
		RESTServer::createMessageError(response,"query: value");
		return 400;
	}
	std::vector<DB::data> datas=DB::getValueSession(queryIterator->second);
	int ret=204;
	if(!datas.empty()){
		ret=200;
		auto &allocator=response.GetAllocator();
		response.SetObject();
		rapidjson::Document sessions;
		sessions.SetArray();
		for(DB::data &data:datas){
			rapidjson::Value session;
			session.SetObject();
			session.AddMember("id",data.session, allocator);
			session.AddMember("expireepoch",data.expireTime, allocator);
			session.AddMember("useragent",data.userAgent, allocator);
			session.AddMember("value",data.value, allocator);
			sessions.PushBack(session, allocator);
		}
		response.AddMember("sessions", sessions, allocator);
	}
	return ret;
}

int RESTServer::databaseSessionsCountGET(rapidjson::Document &response){
	Log::log(Log::debug,this,INFO_LOG,"GET /database/sessions/count");
	auto &allocator=response.GetAllocator();
	response.SetObject();
	response.AddMember("count",DB::countSessions(),allocator);
	response.AddMember("maxcount",Config::getSizeDB(),allocator);
	return 200;
}

RESTServer::RESTServer():
        AbstractServer(Config::getPort()){
	Log::log(Log::info,this,INFO_LOG,"Iniciando servidor REST");
}

RESTServer::enumNodes RESTServer::getEnumNode(std::string node){
	auto iterator=RESTServer::mapNodes.find(node);
	RESTServer::enumNodes enumNode=RESTServer::otherNode;
	if (iterator!=RESTServer::mapNodes.end()){
		enumNode=iterator->second;
	}
	return enumNode;
}

RESTServer::enumMethods RESTServer::getEnumMethod(std::string method){
	auto iterator=RESTServer::mapMethods.find(method);
	RESTServer::enumMethods enumMethod=RESTServer::otherMethod;
	if (iterator!=RESTServer::mapMethods.end()){
		enumMethod=iterator->second;
	}
	return enumMethod;
}

bool RESTServer::connection(AbstractServer::Response &response,AbstractServer::Request &request){
	Log::log(Log::debug,this,INFO_LOG,"Conexion establecida");
    response.headers["Content-Type"]="application/json";
    response.code=404;
    std::vector<std::string> nodes=this->urlToNodes(request.url);
    switch(nodes.size()){
        case 2:{
        	switch(RESTServer::getEnumNode(nodes[0])){
				case RESTServer::database:{
					switch(RESTServer::getEnumNode(nodes[1])){
						case RESTServer::session:{
							switch(RESTServer::getEnumMethod(request.method)){
								case RESTServer::post:{
									rapidjson::Document requestDoc;
									requestDoc.Parse(request.body);
									rapidjson::Document responseDoc;
									response.code=this->databaseSessionPOST(responseDoc,requestDoc,request.headers);
									response.body=RESTServer::documentToString(responseDoc);
								}break;
								default:{
									Log::log(Log::warn,this,INFO_LOG,"Metodo invalido");
									response.code=405;
								}
							}
						}break;
						case RESTServer::config:{
							switch(RESTServer::getEnumMethod(request.method)){
								case RESTServer::get:{
									response.body=this->databaseConfigGET();
									response.code=200;
								}break;
								default:{
									Log::log(Log::warn,this,INFO_LOG,"Metodo invalido");
									response.code=405;
								}
							}
						}break;
						case RESTServer::devices:{
							switch(RESTServer::getEnumMethod(request.method)){
								case RESTServer::get:{
									response.body=this->databaseDevicesGET();
									response.code=200;
								}break;
								default:{
									Log::log(Log::warn,this,INFO_LOG,"Metodo invalido");
									response.code=405;
								}
							}
						}break;
						case RESTServer::sessions:{
							switch(RESTServer::getEnumMethod(request.method)){
								case RESTServer::get:{
									rapidjson::Document responseDoc;
									response.code=this->databaseSessionsGET(responseDoc,request.querys);
									response.body=RESTServer::documentToString(responseDoc);
								}break;
								default:{
									Log::log(Log::warn,this,INFO_LOG,"Metodo invalido");
									response.code=405;
								}
							}
						}break;
					}
				}break;
        	}
        }break;
        case 3:{
        	switch(RESTServer::getEnumNode(nodes[0])){
        		case RESTServer::database:{
        			switch(RESTServer::getEnumNode(nodes[1])){
        				case RESTServer::session:{
        					switch(RESTServer::getEnumMethod(request.method)){
        						case RESTServer::get:{
        							rapidjson::Document responseDoc;
        							response.code=this->databaseSessionGET(responseDoc,nodes[2],request.headers);
        							response.body=RESTServer::documentToString(responseDoc);
        						}break;
        						case RESTServer::patch:{
        							rapidjson::Document requestDoc;
        							requestDoc.Parse(request.body);
        							rapidjson::Document responseDoc;
        							response.code=this->databaseSessionPATCH(responseDoc,requestDoc,nodes[2],request.headers);
        							response.body=RESTServer::documentToString(responseDoc);
        						}break;
        						case RESTServer::deleete:{
        							rapidjson::Document responseDoc;
        							response.code=this->databaseSessionDELETE(responseDoc,nodes[2],request.headers);
        							response.body=RESTServer::documentToString(responseDoc);
        						}break;
        						default:{
        							Log::log(Log::warn,this,INFO_LOG,"Metodo invalido");
        							response.code=405;
        						}
        					}
        				}break;
        				case RESTServer::sessions:{
        					switch(RESTServer::getEnumNode(nodes[2])){
								case RESTServer::count:{
									switch(RESTServer::getEnumMethod(request.method)){
										case RESTServer::get:{
											rapidjson::Document responseDoc;
											response.code=this->databaseSessionsCountGET(responseDoc);
											response.body=RESTServer::documentToString(responseDoc);
										}break;
										default:{
											Log::log(Log::warn,this,INFO_LOG,"Metodo invalido");
											response.code=405;
										}
									}
								}
							}
        				}break;
        			}
        		}break;
        	}
        }break;
    }
    return true;
}

