cmake_minimum_required(VERSION 4.0)

project(gmm2json 
  VERSION 1.0.0 
  DESCRIPTION "Converter to json for Gridmonger's files"
  HOMEPAGE_URL "https://github.com/Jagholin/gmm_reader"
  LANGUAGES C)

find_package(json-c CONFIG)

add_executable(gmm2json defs.c gmm_file.c main.c)

target_link_libraries(gmm2json PRIVATE json-c::json-c)

