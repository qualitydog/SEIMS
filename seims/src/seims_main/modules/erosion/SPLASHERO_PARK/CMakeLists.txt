MESSAGE(STATUS "        Compiling erosion_subdir: SplashEro_Park...")
SET(SRC_LIST ${MODULE_SRC} SplashEro_Park.cpp api.cpp)
ADD_LIBRARY(SPLASHERO_PARK SHARED ${SRC_LIST})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
TARGET_LINK_LIBRARIES(SPLASHERO_PARK util data bmps module_setting)
INSTALL(TARGETS SPLASHERO_PARK DESTINATION ${INSTALL_DIR})
IF (MSVC)
    SET_PROPERTY(TARGET SPLASHERO_PARK PROPERTY FOLDER "modules/erosion")
ENDIF ()
