MESSAGE(STATUS "        Compiling erosion_subdir: KinWavSed_OL...")
SET(SRC_LIST ${MODULE_SRC} KinWavSed_OL.cpp api.cpp)
ADD_LIBRARY(KINWAVSED_OL SHARED ${SRC_LIST})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
TARGET_LINK_LIBRARIES(KINWAVSED_OL util data bmps module_setting)
INSTALL(TARGETS KINWAVSED_OL DESTINATION ${INSTALL_DIR})
IF (MSVC)
    SET_PROPERTY(TARGET KINWAVSED_OL PROPERTY FOLDER "modules/erosion")
ENDIF ()
