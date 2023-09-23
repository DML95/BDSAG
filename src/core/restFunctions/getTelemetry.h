#ifndef CORE_RESTFUNCTIONS_GETTELEMETRY_H_
#define CORE_RESTFUNCTIONS_GETTELEMETRY_H_

	#include "restFunctionCommun.h"

	#include <shared_mutex>

	class GetTelemetry: public RESTFunctionCommun{
		private:
			typedef struct Operation{
				const std::string &method;
				const std::string &path;
				const unsigned int callspersecond;
			};

			std::atomic_bool runThread;
			std::unique_ptr<std::thread> thread;
			std::shared_mutex mutex;
			std::vector<Operation> operations;

			void threadInit();
			void updateTelemetry();
		public:
			GetTelemetry();
			virtual ~GetTelemetry();

			bool filter(Constant::methods method,const std::vector<Constant::nodes> &nodes);
			int operation(Response &response,const Request &request);

			void init(RESTServer &restServer) override;
			void end() override;
	};

#endif /* CORE_RESTFUNCTIONS_GETTELEMETRY_H_ */
