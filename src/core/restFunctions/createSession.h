#ifndef CORE_RESTFUNTIONS_CREATESESSION_H_
#define CORE_RESTFUNTIONS_CREATESESSION_H_

	#include "restFunctionCommun.h"

	//crea una sesion
	class CreateSession: public RESTFunctionCommun {
		public:
			CreateSession();

			bool filter(Constant::methods method,const std::vector<Constant::nodes> &nodes);
			int operation(Response &response,const Request &request);
	};

#endif /* CORE_RESTFUNTIONS_CREATESESSION_H_ */
