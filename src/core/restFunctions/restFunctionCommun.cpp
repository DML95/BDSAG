#include "restFunctionCommun.h"

void RESTFunctionCommun::createMessageError(rapidjson::Document &document,const std::string &error){
    auto &allocator=document.GetAllocator();
    document.SetObject();
    document.AddMember("error",error,allocator);
}
