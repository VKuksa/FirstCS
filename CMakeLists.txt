cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-std=c++11")

project(gl_first_project)

if(WIN32)
	set(source_sr  main_server_windows.cpp win_serv.cpp win_serv.h)
	set(source_cl  main_client_windows.cpp win_clnt.cpp win_clnt.h)
elseif (UNIX)
	set(source_sr  main_server_unix.cpp)
	set(soutce_cl  main_client_unix.cpp)
endif ()


add_executable(gl_first_project_server ${source_sr})
add_executable(gl_first_project_client ${source_cl})


if(WIN32)
	target_link_libraries(gl_first_project_server ws2_32.lib)
	target_link_libraries(gl_first_project_client ws2_32.lib)
endif ()