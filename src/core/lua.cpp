#include "lua.h"

#include"../utils/exception.h"

Lua::Lua(){
	Log::log(Log::trace,this,INFO_LOG,"Iniciando lua");
	this->lua=::luaL_newstate();
	if(!this->lua)throw Exception(0,"No se ha podido iniciar lua",this,INFO_LOG);
	::luaL_openlibs(this->lua);
}

Lua::~Lua() {
	Log::log(Log::trace,this,INFO_LOG,"Finalizando lua");
	::lua_close(this->lua);
}

void Lua::setCode(std::string code){
	Log::log(Log::trace,this,INFO_LOG,"Cargando codigo lua");
	if(int ret=::luaL_loadstring(this->lua,code.data())){
		throw Exception(ret,"No se ha podido cargar el codigo lua",this,INFO_LOG);
	}
	::lua_pcall(this->lua, 0, 0, 0);
}

void Lua::pushGlobalName(std::string name){
	int ret=::lua_getglobal(this->lua,name.data());
	if(!ret){
	    throw Exception(ret,"No se ha encontrado en lua el metodo: "+name,this,INFO_LOG);
	}
}

void Lua::push(std::string &value){
	::lua_pushstring(this->lua,value.data());
}

void Lua::call(int returnArgs){
	::lua_pcall(this->lua,lua_gettop(this->lua)-1,returnArgs,0);
}

long long Lua::getInteger(int index){
	if(!::lua_isinteger(this->lua,index)){
		throw Exception(0,"Valor no es un integer",this,INFO_LOG);
	}
	return ::lua_tointeger(this->lua,index);
}

std::string Lua::getString(int index){
	if(!::lua_isstring(this->lua,index)){
		throw Exception(0,"Valor no es un string",this,INFO_LOG);
	}
	return std::string(::lua_tostring(this->lua,index));
}

bool Lua::getBoolean(int index){
	if(!lua_isboolean(this->lua,index)){
		throw Exception(0,"Valor no es un boolean",this,INFO_LOG);
	}
	return ::lua_toboolean(this->lua,index);
}

void Lua::pop(){
	::lua_pop(this->lua,-1);
}

lua_State& Lua::get(){
	return *this->lua;
}
