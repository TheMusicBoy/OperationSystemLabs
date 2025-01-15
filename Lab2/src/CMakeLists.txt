add_subdirectory(subprocess)

set(INCROOT "${PROJECT_SOURCE_DIR}/include")
set(SRCROOT "${PROJECT_SOURCE_DIR}/src")

set(SRC
    ${SRCROOT}/main.cpp
)

if (NOT "${SRC}" STREQUAL "")
    message(STATUS "Building main executable...")
    add_executable(main ${SRC})
    target_include_directories(main PUBLIC ${INCROOT})
    target_link_libraries(main PUBLIC subprocess)
    set_target_properties(main PROPERTIES LINKER_LANGUAGE CXX)
else()
    message(WARNING "Main executable is not built")
endif()


set(SRC
    ${SRCROOT}/test1.cpp
)

if (NOT "${SRC}" STREQUAL "")
    message(STATUS "Building test1 executable...")
    add_executable(test1 ${SRC})
    target_include_directories(test1 PUBLIC ${INCROOT})
    set_target_properties(test1 PROPERTIES LINKER_LANGUAGE CXX)
else()
    message(WARNING "Test 1 executable not built!")
endif()


set(SRC
    ${SRCROOT}/test2.cpp
)

if (NOT "${SRC}" STREQUAL "")
    message(STATUS "Building test2 executable...")
    add_executable(test2 ${SRC})
    target_include_directories(test2 PUBLIC ${INCROOT})
    set_target_properties(test2 PROPERTIES LINKER_LANGUAGE CXX)
else()
    message(WARNING "Test 2 executable not built!")
endif()
