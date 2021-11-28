#ifndef CORE_RESTFUNTIONS_GETSESSION_H_
#define CORE_RESTFUNTIONS_GETSESSION_H_

	#include "restFunctionCommun.h"

	//busca una sesion
	class GetSession: public RESTFunctionCommun {
		public:
			bool filter(Constant::methods method,const std::vector<Constant::nodes> &nodes);
			int operation(Response &response,const Request &request);
	};

#endif /* CORE_RESTFUNTIONS_GETSESSION_H_ */
