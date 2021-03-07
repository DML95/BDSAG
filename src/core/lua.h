#ifndef CORE_LUA_H_
#define CORE_LUA_H_

	extern "C" {
		#include <lua.h>
		#include <lualib.h>
  	  	#include <lauxlib.h>
	}

	#include<string>

	class Lua {
		private:
			lua_State *lua;
		public:
			//constructores
			Lua();
			//destructor
			~Lua();

			//agerga el codigo lua
			void setCode(std::string code);
			//agrega el nombre de la funcion a ejecuatar
			void pushGlobalName(std::string name);
			//agrega in parametro string
			void push(std::string &value);
			//ejecuta los datos que hay en la pila
			void call(int returnArgs);
			//obtiene un integer
			long long getInteger(int index);
			//obtiene un string
			std::string getString(int index);
			//obtiene un boolean
			bool getBoolean(int index);
			//vacia la pila
			void pop();
			lua_State& get();
	};

#endif /* CORE_LUA_H_ */
