#ifndef UTILS_CONSTANT_H_
#define UTILS_CONSTANT_H_

	#include <unordered_map>

	class Constant {
		private:
			Constant();
		public:
			enum methods {
				otherMethod = -1,
				get,
				post,
				patch,
				deleete,
			};

			static std::unordered_map<std::string,methods> mapMethods;

			enum nodes{
			    otherNode=-1,
			    database,
				session,
				config,
				devices,
				sessions,
				count,
				telemetry,
			};

			static std::unordered_map<std::string,nodes> mapNodes;
	};

#endif /* UTILS_CONSTANT_H_ */
