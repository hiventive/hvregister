# Block

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(${CMAKE_PROJECT_NAME_LOWER} ${SOURCE_FILES})

get_target_property(SystemC_INCLUDE_DIRS SystemC::systemc INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(${CMAKE_PROJECT_NAME_LOWER} PUBLIC ${SystemC_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME_LOWER}
    communication
    configuration
    module
    common
    SystemC::systemc
    cciapi)

# Install

file(GLOB ${CMAKE_PROJECT_NAME_LOWER}_PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
set_target_properties(${CMAKE_PROJECT_NAME_LOWER} PROPERTIES
        PUBLIC_HEADER "${${CMAKE_PROJECT_NAME_LOWER}_PUBLIC_HEADERS}")
install(TARGETS ${CMAKE_PROJECT_NAME_LOWER}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/${CMAKE_PROJECT_NAME_LOWER})

