#ifndef _LUA_API_H_
#define _LUA_API_H_

#include "lua/lua.hpp"

class LuaApi
{
public:
    LuaApi();
    ~LuaApi();
    
    int LoadFile(const char *filename);
    void Call (const char *func, const char *sig, ...);
protected:
private:
    void Error (const char *fmt, ...);

    lua_State *L_;
};

#endif //_LUA_API_H_
