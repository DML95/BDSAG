#ifndef CORE_RESTFUNTIONS_DELETESESSION_H_
#define CORE_RESTFUNTIONS_DELETESESSION_H_

	#include "restFunctionCommun.h"

	//elimina una sesion
	class DeleteSession: public RESTFunctionCommun {
		public:
			DeleteSession();

			bool filter(Constant::methods method,const std::vector<Constant::nodes> &nodes);
			int operation(Response &response,const Request &request);
	};

#endif /* CORE_RESTFUNTIONS_DELETESESSION_H_ */
