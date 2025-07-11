add_executable(jtok_tests
        test_tokenizer.c
)

target_link_libraries(jtok_tests jtok)

add_test(NAME tokenizer_tests COMMAND jtok_tests)
