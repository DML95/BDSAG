#ifndef CORE_RESTFUNTIONS_GETDEVICES_H_
#define CORE_RESTFUNTIONS_GETDEVICES_H_

	#include "restFunctionCommun.h"

	//devuelve los dispostivos asociados a la BD
	class GetDevices: public RESTFunctionCommun {
		public:
			GetDevices();

			bool filter(Constant::methods method,const std::vector<Constant::nodes> &nodes);
			int operation(Response &response,const Request &request);
	};

#endif /* CORE_RESTFUNTIONS_GETDEVICES_H_ */
