# define the executable target
set (EXE_TARGET "up_pqied")
#message("EXE_TARGET: ${EXE_TARGET}")

#include_directories (lua_api lua_api/lua)
include_directories (lua_api)
aux_source_directory(lua_api LUA_SRC)
add_library(lua STATIC IMPORTED)
set_property(TARGET lua PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lua_api/lua/liblua52.a)
set (EXTRA_LIBS ${EXTRA_LIBS} lua)

set (SRC_LIST ${SRC_LIST} ${LUA_SRC} main.cpp ssh_api.cpp)
#set (EXTRA_LIBS ${EXTRA_LIBS} pthread dl)
# add the executable
add_executable (${EXE_TARGET} ${SRC_LIST})
target_link_libraries (${EXE_TARGET} ${EXTRA_LIBS})

# Set the GCC Additional flags to use when compiling this target's sources
# -O0 -O1 -O2 -O3 ...
set_property(TARGET ${EXE_TARGET} PROPERTY COMPILE_FLAGS -O2)

# Set the GCC Additional flags to use when linking this target
# -shared-libgcc -static-libstdc++ -static-libgcc ...
set_property(TARGET ${EXE_TARGET} PROPERTY LINK_FLAGS -static)
#set_target_properties(${EXE_TARGET} PROPERTIES LINK_FLAGS -static-libgcc LINK_FLAGS -static-libstdc++)

# specifies the directory into which runtime target files should be built
#set_property( TARGET ${EXE_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY
#              ${PROJECT_BINARY_DIR}/bin )

# add the install targets
install (TARGETS ${EXE_TARGET} DESTINATION ${PROJECT_SOURCE_DIR}/release)
# install (FILES "${PROJECT_BINARY_DIR}/bin/.sys/preprocess.lua" DESTINATION ${PROJECT_SOURCE_DIR}/release)
install(CODE "execute_process(COMMAND bash lua2lc.sh WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/lua)")
#install (SCRIPT "${PROJECT_SOURCE_DIR}/release/lua2lc.cmake")
