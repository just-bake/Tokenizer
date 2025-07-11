add_executable(tokenize_file
        tokenize_file.c
)

target_link_libraries(tokenize_file jtok)

add_executable(tokenize_example
        tokenize_example.c
)

target_link_libraries(tokenize_example jtok)
