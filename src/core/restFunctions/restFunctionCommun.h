#ifndef CORE_RESTFUNTIONS_RESTFUNTIONSCOMMUN_H_
#define CORE_RESTFUNTIONS_RESTFUNTIONSCOMMUN_H_

	//dependencia circular
	class RESTServer;

	#include<vector>
	#include<map>
	#include <shared_mutex>
	#include <atomic>

	#include"../../utils/constant.h"
	#include"../../apiRapidjson.h"
	#include"../restServer.h"

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
		private:
			std::shared_mutex mutex;
			std::atomic_uint count;
		protected:
			const std::string method;
			const std::string path;
			RESTServer *restServer;

			RESTFunctionCommun(const std::string &method,const std::string &path);

			virtual int operation(Response &response,const Request &request);
		public:

			virtual ~RESTFunctionCommun() = default;

			virtual void init(RESTServer &restServer);
			virtual void end();

			virtual bool filter(const Constant::methods method,const std::vector<Constant::nodes> &nodes);
			int operationAndTelemetry(Response &response,const Request &request);

			unsigned int getAndResetCount();
			const std::string& getMethod();
			const std::string& getPath();

			static void createMessageError(rapidjson::Document &document,const std::string &error);
	};

#endif /* CORE_RESTFUNTIONS_COMMUN_H_ */
