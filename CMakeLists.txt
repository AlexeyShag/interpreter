cmake_minimum_required(VERSION 3.5)
project(Interpret)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-nullability-completeness -Wno-error=sign-conversion -Wall -Werror -Wno-return-type-c-linkage -msha")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")

option(CODE_COVERAGE "CODE_COVERAGE" OFF)
if (CODE_COVERAGE)
    message("Building with code coverage flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
endif()

set(SRC
    ${PROJECT_SOURCE_DIR}/src/combinators/alternative/alternative.cpp
    ${PROJECT_SOURCE_DIR}/src/combinators/base_combinator/base_combinator.cpp
    ${PROJECT_SOURCE_DIR}/src/combinators/combinator/combinator.cpp
    ${PROJECT_SOURCE_DIR}/src/combinators/concator/concator.cpp
    ${PROJECT_SOURCE_DIR}/src/combinators/optional/optional.cpp
    ${PROJECT_SOURCE_DIR}/src/combinators/phrase/phrase.cpp
    ${PROJECT_SOURCE_DIR}/src/combinators/separator/separator.cpp
    ${PROJECT_SOURCE_DIR}/src/combinators/processor/processor.cpp
    ${PROJECT_SOURCE_DIR}/src/combinators/repeat/repeat.cpp
    ${PROJECT_SOURCE_DIR}/src/combinators/reserved/reserved.cpp
    ${PROJECT_SOURCE_DIR}/src/combinators/tag/tag.cpp
    ${PROJECT_SOURCE_DIR}/src/combinators/lazy/lazy.cpp
    ${PROJECT_SOURCE_DIR}/src/lexer/lexer.cpp
    ${PROJECT_SOURCE_DIR}/src/evaluator/expression/expression.cpp
    ${PROJECT_SOURCE_DIR}/src/evaluator/statement/statement.cpp
    ${PROJECT_SOURCE_DIR}/src/languge/parser.cpp
    ${PROJECT_SOURCE_DIR}/FinitStateMachine/finite_state_machine/finite_state_machine.cpp
    )

include_directories(./src ./FinitStateMachine)

set(LIB_NAME inter)
add_library(${LIB_NAME} SHARED ${SRC})
target_include_directories(${LIB_NAME} PUBLIC .)

add_executable(interpreter ./main.cpp)

target_link_libraries(main PUBLIC ${LIB_NAME})

add_executable(tests
                tests/main.cpp 
                tests/combinators.cpp
                tests/stress_test.cpp
                ${PROJECT_SOURCE_DIR}/FinitStateMachine/tests/finite_state_machine.cpp
)


target_link_libraries(tests ${LIB_NAME})

add_subdirectory(gtest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include PUBLIC ${gtest_SOURCE_DIR})

target_link_libraries(tests gtest pthread )

target_include_directories(tests PRIVATE ${gtest_INCLUE_DIR})