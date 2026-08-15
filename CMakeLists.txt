cmake_minimum_required(VERSION 3.16)
project(StudentuInformacineSistema VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build tipas" FORCE)
endif()

# Parametras: naudoti std::vector vietoj nuosavos Vector klasės
option(NAUDOTI_STD_VECTOR "Naudoti std::vector vietoj nuosavos Vector" OFF)

# =====================================================================
# Bendri kompiliavimo parametrai
# =====================================================================
function(nustatyk_parametrus taikinys)
    if(MSVC)
        target_compile_options(${taikinys} PRIVATE /W4 /O2 /utf-8)
    else()
        target_compile_options(${taikinys} PRIVATE -Wall -Wextra -O2)
    endif()
    if(NAUDOTI_STD_VECTOR)
        target_compile_definitions(${taikinys} PRIVATE NAUDOTI_STD_VECTOR)
    endif()
endfunction()

# =====================================================================
# Klasių biblioteka
# =====================================================================
add_library(studentaiLib STATIC
    OOP_2/zmogus.cpp
    OOP_2/studentas.cpp
)
target_include_directories(studentaiLib PUBLIC ${CMAKE_SOURCE_DIR}/OOP_2)
nustatyk_parametrus(studentaiLib)

# =====================================================================
# Pagrindinė programa
# =====================================================================
add_executable(programa
    OOP_2/OOP_2.cpp
    OOP_2/testStudentas.cpp
    OOP_2/menu.cpp
    OOP_2/enter.cpp
    OOP_2/generate.cpp
    OOP_2/file.cpp
    OOP_2/print.cpp
    OOP_2/test.cpp
)
target_link_libraries(programa PRIVATE studentaiLib)
nustatyk_parametrus(programa)

if(EXISTS ${CMAKE_SOURCE_DIR}/OOP_2/data)
    add_custom_command(TARGET programa POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/OOP_2/data
            $<TARGET_FILE_DIR:programa>/data
        COMMENT "Kopijuojamas data/ aplankas"
    )
endif()

# =====================================================================
# Spartos analizė (benchmark) — nereikalauja Catch2
# =====================================================================
add_executable(benchmark tests/benchmark.cpp)
target_include_directories(benchmark PRIVATE ${CMAKE_SOURCE_DIR}/OOP_2)
nustatyk_parametrus(benchmark)

# =====================================================================
# Unit testai (Catch2)
# =====================================================================
if(EXISTS ${CMAKE_SOURCE_DIR}/tests/catch.hpp)
    enable_testing()

    # Studento ir zmogus klasių testai
    add_executable(unitTests tests/unitTests.cpp)
    target_link_libraries(unitTests PRIVATE studentaiLib)
    target_include_directories(unitTests PRIVATE ${CMAKE_SOURCE_DIR}/tests)
    nustatyk_parametrus(unitTests)

    # Vector klasės testai
    add_executable(vectorTests tests/vectorTests.cpp)
    target_include_directories(vectorTests PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
        ${CMAKE_SOURCE_DIR}/OOP_2)
    nustatyk_parametrus(vectorTests)

    add_test(NAME KlasiuTestai COMMAND unitTests)
    add_test(NAME VectorTestai COMMAND vectorTests)
    add_test(NAME RuleOfFive COMMAND unitTests "[rule5]")
    add_test(NAME VectorTalpa COMMAND vectorTests "[talpa]")

    message(STATUS "Unit testai ijungti")
else()
    message(WARNING
        "tests/catch.hpp nerastas - unit testai NEBUS kompiliuojami.\n"
        "  https://github.com/catchorg/Catch2/releases/download/v2.13.10/catch.hpp")
endif()

# =====================================================================
# Doxygen dokumentacija
# =====================================================================
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generuojama Doxygen dokumentacija"
        VERBATIM
    )
endif()

# =====================================================================
# Diegimas (CPack) — sukuria setup.exe
# =====================================================================
install(TARGETS programa
    RUNTIME DESTINATION bin
)

if(EXISTS ${CMAKE_SOURCE_DIR}/OOP_2/data)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/OOP_2/data/
        DESTINATION bin/data
        FILES_MATCHING PATTERN "studentai10000.txt" PATTERN "studentai100000.txt"
    )
endif()

install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION .)
if(EXISTS ${CMAKE_SOURCE_DIR}/LICENSE)
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION .)
endif()

set(CPACK_PACKAGE_NAME "Studentu informacine sistema")
set(CPACK_PACKAGE_VENDOR "VU")
set(CPACK_PACKAGE_VERSION "3.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Studentu duomenu apdorojimo sistema")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "VU/Marius-Augustinas")

# NSIS nustatymai (Windows setup.exe)
set(CPACK_NSIS_DISPLAY_NAME "Studentu informacine sistema")
set(CPACK_NSIS_PACKAGE_NAME "Studentu informacine sistema")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH OFF)

# Administravimo teisės
set(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS "RequestExecutionLevel admin")

# Nuorodos Start meniu ir darbalaukyje
set(CPACK_PACKAGE_EXECUTABLES "programa" "Studentu sistema")
set(CPACK_CREATE_DESKTOP_LINKS "programa")

set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$DESKTOP\\\\Studentu sistema.lnk' '$INSTDIR\\\\bin\\\\programa.exe'"
)
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$DESKTOP\\\\Studentu sistema.lnk'"
)

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)