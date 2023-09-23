#include "constant.h"

std::unordered_map<std::string,Constant::methods> Constant::mapMethods={
	{"GET",get},
	{"POST",post},
	{"PATCH",patch},
	{"DELETE",deleete},
};

std::unordered_map<std::string,Constant::nodes> Constant::mapNodes={
	{"database",database},
	{"session",session},
	{"config",config},
	{"devices",devices},
	{"sessions",sessions},
	{"count",count},
	{"telemetry",telemetry},
};
