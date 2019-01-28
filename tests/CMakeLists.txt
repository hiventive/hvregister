# Tests

file(GLOB TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${CMAKE_PROJECT_NAME_LOWER}-tests ${TEST_FILES})
target_include_directories(${CMAKE_PROJECT_NAME_LOWER}-tests PUBLIC ${GTEST_INCLUDE_DIRS})

set(${CMAKE_PROJECT_NAME_LOWER}-LIBRARIES ${CMAKE_PROJECT_NAME_LOWER}
		${GTEST_LIBRARIES}
	communication
	configuration
	module
    common
    cciapi
    SystemC::systemc)

# Disable SPDLog with VS 2012
if(NOT MSVC)
	list(APPEND ${CMAKE_PROJECT_NAME_LOWER}-LIBRARIES Threads::Threads)
endif()

target_link_libraries(${CMAKE_PROJECT_NAME_LOWER}-tests ${${CMAKE_PROJECT_NAME_LOWER}-LIBRARIES})

add_test(NAME ${CMAKE_PROJECT_NAME}Tests COMMAND ${CMAKE_PROJECT_NAME_LOWER}-tests)
