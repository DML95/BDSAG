#ifndef CORE_RESTFUNTIONS_COUNTSESSIONS_H_
#define CORE_RESTFUNTIONS_COUNTSESSIONS_H_

	#include "restFunctionCommun.h"

	//cuanta el numero de sesiones creadas
	class CountSessions: public RESTFunctionCommun {
		public:
			CountSessions();

			bool filter(Constant::methods method,const std::vector<Constant::nodes> &nodes);
			int operation(Response &response,const Request &request);
	};

#endif /* CORE_RESTFUNTIONS_COUNTSESSIONS_H_ */
