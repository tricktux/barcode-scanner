# All the source files for the bot.
file(GLOB SOURCES "*.cpp")

include_directories(${CMAKE_SOURCE_DIR}/inc)
include_directories(${OpenCV_INCLUDE_DIRS}) # Not needed for CMake >= 2.8.11

if (GLOG_FOUND)
	include_directories(SYSTEM "${GLOG_INCLUDE_DIRS}")
endif ()

if (RAPIDJSON_INCLUDE_DIRS)
	include_directories(SYSTEM "${RAPIDJSON_INCLUDE_DIRS}")
endif ()

# Show more warnings at compiletime.
if (MSVC)
    # FIXME: put Windows specific options here.
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif ()

# Set barcode-scanner.
add_executable(barcode-scanner ${SOURCES})

if (GLOG_FOUND)
	target_link_libraries(barcode-scanner ${GLOG_LIBRARIES})
endif ()

if (APPLE)
	target_link_libraries(barcode-scanner "-framework Carbon")
endif ()

# Linux specific.
if (UNIX AND NOT APPLE)
	target_link_libraries(barcode-scanner pthread dl)
endif ()

if (ZBAR_FOUND)
	target_link_libraries (barcode-scanner ${OpenCV_LIBRARIES} ${ZBAR_LIBRARIES})
endif ()
target_link_libraries(barcode-scanner ${OpenCV_LIBS})
