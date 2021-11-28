#ifndef CORE_RESTFUNTIONS_UPDATESESSION_H_
#define CORE_RESTFUNTIONS_UPDATESESSION_H_

	#include "restFunctionCommun.h"

	//modifica una sesion
	class UpdateSession: public RESTFunctionCommun {
		public:
			bool filter(Constant::methods method,const std::vector<Constant::nodes> &nodes);
			int operation(Response &response,const Request &request);
	};

#endif /* CORE_RESTFUNTIONS_UPDATESESSION_H_ */
