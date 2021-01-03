cmake_minimum_required(VERSION 3.10)

project(wsdisasm)

# parse the current version
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/src/wsdisasm.cpp" wsdisasm_version_defines
     REGEX "#define VERSION_(MAJOR|MINOR|PATCH)")
foreach(ver ${wsdisasm_version_defines})
    if(ver MATCHES "#define VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
message(STATUS "wsdisasm version ${VERSION}")

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(wsdisasm src/wsdisasm.cpp src/disasm/dis_decode.cpp src/disasm/dis_groups.cpp src/disasm/resolve.cpp src/disasm/syntax.cpp src/disasm/osdep.cpp)
