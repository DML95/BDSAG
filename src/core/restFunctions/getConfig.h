#ifndef CORE_RESTFUNTIONS_GETCONFIG_H_
#define CORE_RESTFUNTIONS_GETCONFIG_H_

	#include "restFunctionCommun.h"

	//devuelve la configuaracion actual de la BD
	class GetConfig: public RESTFunctionCommun {
		public:
			GetConfig();

			bool filter(Constant::methods method,const std::vector<Constant::nodes> &nodes);
			int operation(Response &response,const Request &request);
	};

#endif /* CORE_RESTFUNTIONS_GETCONFIG_H_ */
