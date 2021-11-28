#ifndef CORE_RESTFUNTIONS_GETSESSIONS_H_
#define CORE_RESTFUNTIONS_GETSESSIONS_H_

	#include "restFunctionCommun.h"

	//devuelve las sesiones que su valor corresponda con la expresion regular
	class GetSessions: public RESTFunctionCommun {
		public:
			bool filter(Constant::methods method,const std::vector<Constant::nodes> &nodes);
			int operation(Response &response,const Request &request);
	};

#endif /* CORE_RESTFUNTIONS_GETSESSIONS_H_ */
