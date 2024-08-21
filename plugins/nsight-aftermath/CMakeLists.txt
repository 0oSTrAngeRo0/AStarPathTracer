set(NSIGHT_AFTERMATH_SDK ${CMAKE_CURRENT_LIST_DIR})
set(NSIGHT_AFTERMATH_SDK_TARGET_NAME ${PROJECT_NAME})

if(WIN32 OR UNIX)
  find_path(NsightAftermath_INCLUDE_DIR
    NAMES GFSDK_Aftermath.h
    PATHS
      "${NSIGHT_AFTERMATH_SDK}/include"
    )

  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    find_library(NsightAftermath_LIBRARY
      NAMES GFSDK_Aftermath_Lib.x64
      PATHS
        "${NSIGHT_AFTERMATH_SDK}/lib/x64"
        NO_SYSTEM_ENVIRONMENT_PATH
        )
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    find_library(NsightAftermath_LIBRARY
      NAMES GFSDK_Aftermath_Lib.x86
      PATHS
        "${NSIGHT_AFTERMATH_SDK}/lib/x86"
        NO_SYSTEM_ENVIRONMENT_PATH
        )
  endif()
else()
    find_path(NsightAftermath_INCLUDE_DIR
      NAMES GFSDK_Aftermath.h
      PATHS
        "${NSIGHT_AFTERMATH_SDK}/include")
    find_library(NsightAftermath_LIBRARY
      NAMES GFSDK_Aftermath_Lib
      PATHS
        "${NSIGHT_AFTERMATH_SDK}/lib")
endif()

set(NsightAftermath_LIBRARIES ${NsightAftermath_LIBRARY})
set(NsightAftermath_INCLUDE_DIRS ${NsightAftermath_INCLUDE_DIR})

add_library(NsightAftermath INTERFACE)
add_library(NsightAftermath::NsightAftermath ALIAS NsightAftermath)

target_include_directories(NsightAftermath INTERFACE 
    ${NsightAftermath_LIBRARIES} 
    ${NsightAftermath_INCLUDE_DIRS}
    ${NSIGHT_AFTERMATH_SDK}/wrapper
)

mark_as_advanced(NsightAftermath_INCLUDE_DIR NsightAftermath_LIBRARY)
