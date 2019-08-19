# define the executable target
set (EXE_TARGET "scan_upsys")
#message("EXE_TARGET: ${EXE_TARGET}")

set (SRC_LIST ${SRC_LIST} ${LUA_SRC} scan_upsys.cpp)
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
install (TARGETS ${EXE_TARGET} DESTINATION ${PROJECT_SOURCE_DIR}/release/.sys)
# install (FILES "${PROJECT_BINARY_DIR}/bin/.sys/preprocess.lua" DESTINATION ${PROJECT_SOURCE_DIR}/release)
#install (SCRIPT "${PROJECT_SOURCE_DIR}/release/lua2lc.cmake")
