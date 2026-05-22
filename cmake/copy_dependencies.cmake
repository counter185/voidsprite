function (copy_rt_libs binary_dir)
    file(GET_RUNTIME_DEPENDENCIES RESOLVED_DEPENDENCIES_VAR rtlibs EXECUTABLES "${binary_dir}/voidsprite")
    file(COPY ${rtlibs} DESTINATION "${binary_dir}")
endfunction()

copy_rt_libs(${BINARY_DIR})
