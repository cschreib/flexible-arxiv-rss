cmake_minimum_required(VERSION 2.8)

# project name
project(arxiv-filter CXX)
add_definitions(-std=c++11)
add_definitions(-DZEN_LINUX)
add_definitions(-Wall)

# setup source
include_directories("${CMAKE_SOURCE_DIR}/src")
add_executable(oai-get-preference src/oai-get-preference.cpp)
add_executable(oai-merge-records  src/oai-merge-records.cpp)
add_executable(oai-filter-records src/oai-filter-records.cpp)
add_executable(oai-records-2rss   src/oai-records-2rss.cpp)
add_executable(oai-records-2html  src/oai-records-2html.cpp)

# setup configuration
if (NOT USER_CONFIG_DIR)
    set(USER_CONFIG_DIR $ENV{HOME}/.local/share/arxiv-transform)
endif()
if (NOT USER_HTTP_PORT)
    set(USER_HTTP_PORT 8080)
endif()
if (NOT WWW_PATH)
    set(WWW_PATH ${USER_CONFIG_DIR}/www)
endif()
if (NOT NGINX_CACHE)
    set(NGINX_CACHE ${USER_CONFIG_DIR}/nginx-cache)
endif()

# apply configuration to template config files
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/preferences.xml ${CMAKE_BINARY_DIR}/nginx.conf
    COMMAND ${CMAKE_SOURCE_DIR}/share/make_config.sh
    ${CMAKE_BINARY_DIR} ${USER_CONFIG_DIR} ${USER_HTTP_PORT} ${WWW_PATH} ${NGINX_CACHE})

add_custom_target(generate-config DEPENDS ${CMAKE_BINARY_DIR}/preferences.xml)
add_dependencies(oai-records-2html generate-config)

# install everything in the local share folder
install(FILES share/arxiv.css DESTINATION ${USER_CONFIG_DIR} COMPONENT config)
install(FILES ${CMAKE_BINARY_DIR}/preferences.xml DESTINATION ${USER_CONFIG_DIR} COMPONENT config)
install(FILES ${CMAKE_BINARY_DIR}/nginx.conf DESTINATION ${USER_CONFIG_DIR} COMPONENT config)
install(PROGRAMS ${CMAKE_BINARY_DIR}/oai-get-preference DESTINATION ${USER_CONFIG_DIR} COMPONENT bin)
install(PROGRAMS ${CMAKE_BINARY_DIR}/oai-merge-records DESTINATION ${USER_CONFIG_DIR} COMPONENT bin)
install(PROGRAMS ${CMAKE_BINARY_DIR}/oai-filter-records DESTINATION ${USER_CONFIG_DIR} COMPONENT bin)
install(PROGRAMS ${CMAKE_BINARY_DIR}/oai-records-2rss DESTINATION ${USER_CONFIG_DIR} COMPONENT bin)
install(PROGRAMS ${CMAKE_BINARY_DIR}/oai-records-2html DESTINATION ${USER_CONFIG_DIR} COMPONENT bin)
install(PROGRAMS share/poll_arxiv_oai.sh DESTINATION ${USER_CONFIG_DIR} COMPONENT bin)
install(DIRECTORY nginx-cache DESTINATION ${USER_CONFIG_DIR})
