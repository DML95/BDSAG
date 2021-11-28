#ifndef CORE_RESTFUNTIONS_RESTFUNTIONSCOMMUN_H_
#define CORE_RESTFUNTIONS_RESTFUNTIONSCOMMUN_H_

	#include<vector>
	#include<map>

	#include"../../utils/constant.h"
	#include"../../apiRapidjson.h"

	class RESTFunctionCommun {
		public:

			typedef struct{
				std::vector<std::string> *nodes;
				std::unordered_map<std::string,std::string> *querys;
				std::unordered_map<std::string,std::string> *headers;
				rapidjson::Document *json;
			}Request;

			typedef struct{
				std::map<std::string,std::string> *headers;
				rapidjson::Document *json;
			}Response;

			virtual bool filter(const Constant::methods method,const std::vector<Constant::nodes> &nodes);
			virtual int operation(Response &response,const Request &request);

			static void createMessageError(rapidjson::Document &document,const std::string &error);
	};

#endif /* CORE_RESTFUNTIONS_COMMUN_H_ */
