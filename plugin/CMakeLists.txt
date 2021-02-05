# CMakeList.txt: proyecto de CMake para plugin, incluya el origen y defina
# la lógica específica del proyecto aquí.
#
cmake_minimum_required (VERSION 3.8)

set(libobs_DIR "C:/Users/tikic/Downloads/obs-studio-x64-0.0.0.0-vs2017")
include("${libobs_DIR}/cmake/LibObs/LibObsConfig.cmake")

# Agregue un origen al ejecutable de este proyecto.
set(metaball_SOURCES plugin.c)

add_library(metaballs MODULE ${metaball_SOURCES})
target_link_libraries(metaballs libobs)
target_include_directories(metaballs PRIVATE "${libobs_DIR}/include")


set(OBS_DIR "F:/Aplicacionesl/obs-studio")

# Detect Architecture
math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")

install(TARGETS metaballs
        RUNTIME DESTINATION "${OBS_DIR}/obs-plugins/${BITS}bit/" COMPONENT Runtime
        LIBRARY DESTINATION "${OBS_DIR}/obs-plugins/${BITS}bit/" COMPONENT Runtime)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:metaballs>
		    DESTINATION "${OBS_DIR}/obs-plugins/${BITS}bit/"
		    OPTIONAL)
endif()
	
install(DIRECTORY "${PROJECT_SOURCE_DIR}/data/"
	    DESTINATION "${OBS_DIR}/data/obs-plugins/metaballs/")