set(CMAKE_INCLUDE_CURRENT_DIR ON)

message("**** configuring KratosCoSimulationApplication ****")

if(${CO_SIMULATION_APPLICATION_PYTHON} MATCHES ON)
    message("**** configuring KratosCoSimulationApplication for PYTHON-ONLY ****")
    project (KratosMultiphysics)
    cmake_minimum_required (VERSION 2.8.6)

    get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
    get_filename_component(KRATOS_DIR ${PARENT_DIR} DIRECTORY)

    message(STATUS "Deleting: ${KRATOS_DIR}/KratosMultiphysics")
    file(REMOVE_RECURSE ${KRATOS_DIR}/KratosMultiphysics)
    message(STATUS "Deleting: ${KRATOS_DIR}/libs")
    file(REMOVE_RECURSE ${KRATOS_DIR}/libs)

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/custom_data_structure/KratosMultiphysics.py" DESTINATION ${KRATOS_DIR}/KratosMultiphysics RENAME "__init__.py")
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/custom_data_structure/CoSimulationApplication.py" DESTINATION ${KRATOS_DIR}/KratosMultiphysics/CoSimulationApplication RENAME "__init__.py")

else(${CO_SIMULATION_APPLICATION_PYTHON} MATCHES ON)

    ################### PYBIND11
    include(pybind11Tools)

    include_directories( ${CMAKE_SOURCE_DIR}/kratos )
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/co_sim_io )

    ## CoSimulation Core sources
    file(GLOB_RECURSE KRATOS_CO_SIMULATION_APPLICATION_CORE
        ${CMAKE_CURRENT_SOURCE_DIR}/co_simulation_application.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/co_simulation_application_variables.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/custom_io/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/custom_processes/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/custom_utilities/*.cpp
    )

    ## CoSimulation testing sources
    if(${KRATOS_BUILD_TESTING} MATCHES ON)
        file(GLOB_RECURSE KRATOS_CO_SIMULATION_APPLICATION_TESTING_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp_tests/*.cpp)
    endif(${KRATOS_BUILD_TESTING} MATCHES ON)

    ## CoSimulation python interface sources
    file(GLOB_RECURSE KRATOS_CO_SIMULATION_APPLICATION_PYTHON_INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/custom_python/*.cpp)

    set(KRATOS_CO_SIMULATION_TESTING_CPP_PING_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp_ping_pong_solvers/cpp_test_solver_ping/cpp_test_solver_ping.cpp)
    set(KRATOS_CO_SIMULATION_TESTING_CPP_PONG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp_ping_pong_solvers/cpp_test_solver_pong/cpp_test_solver_pong.cpp)


    add_library(KratosCoSimulationCore SHARED ${KRATOS_CO_SIMULATION_APPLICATION_CORE} ${KRATOS_CO_SIMULATION_APPLICATION_TESTING_SOURCES})
    target_link_libraries(KratosCoSimulationCore PUBLIC KratosCore)
    set_target_properties(KratosCoSimulationCore PROPERTIES COMPILE_DEFINITIONS "CO_SIMULATION_APPLICATION=EXPORT,API")

    ###############################################################
    ## define library Kratos which defines the basic python interface
    pybind11_add_module(KratosCoSimulationApplication MODULE THIN_LTO ${KRATOS_CO_SIMULATION_APPLICATION_PYTHON_INTERFACE})
    target_link_libraries(KratosCoSimulationApplication PRIVATE KratosCoSimulationCore)
    set_target_properties(KratosCoSimulationApplication PROPERTIES PREFIX "")

    # Making the cpp_test_solver_one and cpp_test_solver_two executables
    add_executable(ping ${KRATOS_CO_SIMULATION_TESTING_CPP_PING_SOURCES})
    add_executable(pong ${KRATOS_CO_SIMULATION_TESTING_CPP_PONG_SOURCES})

    # changing the .dll suffix to .pyd (Windows)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set_target_properties(KratosCoSimulationApplication PROPERTIES SUFFIX .pyd)
    endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    # changing the .dylib suffix to .so (OS X)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set_target_properties(KratosCoSimulationApplication PROPERTIES SUFFIX .so)
    endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    # Cotire
    if(USE_COTIRE MATCHES ON)
        cotire(KratosCoSimulationCore)
        cotire(KratosCoSimulationApplication)
    endif(USE_COTIRE MATCHES ON)

    # Add to the KratosMultiphisics Python module
    kratos_python_install(${INSTALL_PYTHON_USING_LINKS} ${CMAKE_CURRENT_SOURCE_DIR}/CoSimulationApplication.py KratosMultiphysics/CoSimulationApplication/__init__.py )

    # Install python files
    get_filename_component (CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    kratos_python_install_directory(${INSTALL_PYTHON_USING_LINKS} ${CMAKE_CURRENT_SOURCE_DIR}/python_scripts KratosMultiphysics/${CURRENT_DIR_NAME} )

    # Kratos Testing. Install everything except sources to ensure that reference and configuration files are copied.
    if(${INSTALL_TESTING_FILES} MATCHES ON )
        get_filename_component (CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests DESTINATION applications/${CURRENT_DIR_NAME}
            PATTERN "*.git" EXCLUDE
            PATTERN "*.c" EXCLUDE
            PATTERN "*.h" EXCLUDE
            PATTERN "*.cpp" EXCLUDE
            PATTERN "*.hpp" EXCLUDE
    )
    endif(${INSTALL_TESTING_FILES} MATCHES ON)

    # Install targets
    install(TARGETS KratosCoSimulationCore DESTINATION libs )
    install(TARGETS KratosCoSimulationApplication DESTINATION libs )

endif(${CO_SIMULATION_APPLICATION_PYTHON} MATCHES ON)
