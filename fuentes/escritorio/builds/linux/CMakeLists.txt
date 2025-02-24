## --------------------------------------------------------------------------------
## Programación del cauce gráfico (Máster en Desarrollo de Software)
##
## Archivo de configuración para compilar usando CMake en Linux
## última modificación: 14 de Diciembre de 2023
##
## --------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.5.1)
project (pracs_ig_linux_exe)

## ----------------------------------------------------------------------
## Definir parámetros configurables

set ( raiz                "../..")
set ( carpeta_fuentes     ${raiz}/src/cpp)
set ( carpeta_ejecutables "../bin" )
set ( nombre_exe_debug    "debug_exe" )
set ( nombre_exe_release  "release_exe" )
set ( opcs_warnings       "-Wall -Wextra -Wno-unused-parameter -Wno-unused-private-field -Wno-missing-field-initializers" )
set ( opcs_primer_error   "-Wfatal-errors")
set ( version_cpp         "-std=c++20")

## ----------------------------------------------------------------------
## definir los flags del compilador

set ( flags_compilador  "${version_cpp} ${opcs_warnings} ${opcs_primer_error}")  
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flags_compilador}")

## ----------------------------------------------------------------------
## definir las listas de archivos *.cpp y *.h que se deben compilar

file (GLOB fuentes_units      ${carpeta_fuentes}/*.cpp)
file (GLOB fuentes_headers    ${carpeta_fuentes}/*.h)

## ----------------------------------------------------------------------
## buscar las librerías que queremos enlazar, decir que enlacen

set( OpenGL_GL_PREFERENCE "GLVND" ) ## usado en 'find_package( OpenGL )',  sirve para seleccionar Nvidia en ordenadores con GPU discreta nvidia e integrada INTEL 

find_package( GLEW      REQUIRED )
find_package( OpenGL    REQUIRED )
find_package( glfw3 3.3 REQUIRED )
link_libraries( glfw GLEW OpenGL::GL jpeg )

## ----------------------------------------------------------------------
## definir las unidades del ejecutable de debug y de release, dar flags específicos

add_executable            ( ${nombre_exe_debug} ${fuentes_units} ${fuentes_headers})
target_include_directories( ${nombre_exe_debug} PUBLIC ${carpeta_fuentes}  )
set_target_properties     ( ${nombre_exe_debug} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${carpeta_ejecutables} )
set_target_properties     ( ${nombre_exe_debug} PROPERTIES COMPILE_FLAGS "-g" )

add_executable            ( ${nombre_exe_release} EXCLUDE_FROM_ALL ${fuentes_units}  ${fuentes_headers})
target_include_directories( ${nombre_exe_release} PUBLIC ${carpeta_fuentes}  )
set_target_properties     ( ${nombre_exe_release} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${carpeta_ejecutables} )
set_target_properties     ( ${nombre_exe_release} PROPERTIES COMPILE_FLAGS "-O2" )

## -------------------------------------------------------------------------------------------------
## imprime todas las variables cuando se pone 'cmake ..' 
## (activar/desactivar poniendo true/false)

if ( false )
   get_cmake_property(_variableNames VARIABLES)
   list (SORT _variableNames)
   foreach (_variableName ${_variableNames})
      message(STATUS "${_variableName}=${${_variableName}}")
   endforeach()
endif()

