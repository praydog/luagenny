# Codegen validation test: compiles the generated .hpp files and verifies layout.
# Requires running the Lua test first to generate headers.

add_executable(codegen_test test/codegen_test.cpp)
target_include_directories(codegen_test PRIVATE test)
target_compile_features(codegen_test PRIVATE cxx_std_20)
if(MSVC)
    target_compile_options(codegen_test PRIVATE /W4)
endif()

# Track header generation via stamp file — avoids maintaining a complete list
# of every generated header.
set(GENERATED_HEADERS_STAMP
    ${CMAKE_CURRENT_BINARY_DIR}/codegen_headers.stamp
)

add_custom_command(
    OUTPUT ${GENERATED_HEADERS_STAMP}
    COMMAND $<TARGET_FILE:repl> --test ${CMAKE_CURRENT_SOURCE_DIR}/test/test.lua
    COMMAND ${CMAKE_COMMAND} -E touch ${GENERATED_HEADERS_STAMP}
    DEPENDS repl ${CMAKE_CURRENT_SOURCE_DIR}/test/test.lua
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating .hpp headers via Lua test"
)
add_custom_target(generate_headers DEPENDS ${GENERATED_HEADERS_STAMP})
add_dependencies(codegen_test generate_headers)

add_test(NAME codegen_test COMMAND codegen_test)
