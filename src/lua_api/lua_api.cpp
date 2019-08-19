#include "lua_api.h"
#include <cstring>
#include <cstdlib>
#include <cstdarg>

using namespace std;

LuaApi::LuaApi ()
{
    L_ = luaL_newstate(); /* opens Lua */
    luaL_openlibs(L_); /* opens the standard libraries */
}

LuaApi::~LuaApi ()
{
    lua_close(L_);
}

void LuaApi::Error (const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    lua_close(L_);
    exit(EXIT_FAILURE);
}
/*!
call lua function

    Input:  func -- function name be called
            sig -- arguments & result summary.
            ... -- arguments & result 
*/
void LuaApi::Call (const char *func, const char *sig, ...)
{
    va_list vl;
    int narg, nres; // number of arguments and results
    va_start(vl, sig);
    lua_getglobal(L_, func); // push function
    if (!lua_isfunction(L_, -1)) Error("%s should be a function\n", func);
    
    for (narg = 0; *sig; narg++) { // repeat for each argument
        luaL_checkstack(L_, 1, "too many arguments");    // check stack space
        switch (*sig++) {
            case 'd': // double argument
                lua_pushnumber(L_, va_arg(vl, double));
                printf("d=%f\n", lua_tonumber(L_, -1));
                break;
            case 'i': // int argument
                lua_pushinteger(L_, va_arg(vl, int));
                //printf("i=%d\n", lua_tointeger(L_, -1));
                break;
            case 's': // string argument
                lua_pushstring(L_, va_arg(vl, char *));
                //printf("s=%s\n", lua_tostring(L_, -1));
                break;
            case '>': // end of arguments
                goto endargs;
            default:
                Error("invalid option (%c)", *(sig - 1));
        }
    }
    endargs:

    nres = strlen(sig); // number of expected results
    if (lua_pcall(L_, narg, nres, 0) != 0) Error("error calling '%s': %s", func, lua_tostring(L_, -1));
    
    nres = -nres; // stack index of first result
    while (*sig) { // repeat for each result
        switch (*sig++) {
            case 'd': { // double result
                int isnum;
                double n = lua_tonumberx(L_, nres, &isnum);
                if (!isnum) Error("wrong result type");
                *va_arg(vl, double *) = n;
                break;
            }
            case 'i': { // int result
                int isnum;
                int n = lua_tointegerx(L_, nres, &isnum);
                if (!isnum) Error("wrong result type");
                *va_arg(vl, int *) = n;
                break;
            }
            case 's': { // string result
                const char *s = lua_tostring(L_, nres); //由于有此项，lua_pop要由调用者来操作
                if (s == NULL) Error("wrong result type");
                *va_arg(vl, const char **) = s;
                break;
            }
            default:
                Error("invalid option (%c)", *(sig - 1));
        }
        nres++;
    }
    //Can't pop the results from the stack. It is up to the caller to pop them, after it finishes using occasional string results    
    va_end(vl);
}

/*!
    Return: 0=success, 1=failure
*/
int LuaApi::LoadFile(const char *filename)
{
    char luafile[128];
    sprintf(luafile, "%s.lc", filename);
    FILE *fp = fopen(luafile, "r");
    if(fp) {
        fclose(fp); 
    } else {
        sprintf(luafile, "%s.lua", filename);
        fp = fopen(luafile, "r");
        if (!fp) return 1;
    }
    if (luaL_loadfile(L_, luafile) || lua_pcall(L_, 0, 0, 0)) {
        Error("cannot run config file: %s", lua_tostring(L_, -1));
        return 1;
    }
    return 0;
}
