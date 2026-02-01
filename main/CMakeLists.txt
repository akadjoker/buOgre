project(main)
cmake_policy(SET CMP0072 NEW)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   -D_CRT_SECURE_NO_WARNINGS")
    if (MSVC)
        if(CMAKE_BUILD_TYPE MATCHES Debug)
            add_compile_options(/RTC1 /Od /Zi)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
        endif()     
    endif()

endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_compile_options(
        # Optimization level
        -O3
        
       
        
        # Architecture specific
        -march=native
        -mtune=native
        

        
        # Vectorization
        -ftree-vectorize
        
        # Strip debug info
        -DNDEBUG
        
        # Inline agressivo
        -finline-functions
        -funroll-loops

)

 

file(GLOB SOURCES "src/*.cpp")
add_executable(main   ${SOURCES})

# Try to find Ogre3D - ignore missing optional components
find_package(OGRE REQUIRED COMPONENTS RTShaderSystem Overlay  Terrain)

find_package(SDL2 REQUIRED)

target_include_directories(main PRIVATE
    ${SDL2_INCLUDE_DIRS}
)

if(OGRE_FOUND)
    # Include Ogre3D headers
    target_include_directories(main PRIVATE ${OGRE_INCLUDE_DIRS})
    
    # Link Ogre3D libraries (only core, skip optional deps)
   target_link_libraries(main
    OgreMain
    OgreRTShaderSystem
    OgreOverlay
 
    OgreTerrain
      ${SDL2_LIBRARIES}
)
    
    # Define a flag for Ogre3D support
    target_compile_definitions(main PRIVATE HAVE_OGRE3D=1)
    message(STATUS "Ogre3D found: OgreMain")
else()
    # Try pkg-config as fallback
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(OGRE QUIET OGRE)
        if(OGRE_FOUND)
            target_include_directories(main PRIVATE ${OGRE_INCLUDE_DIRS})
            target_link_libraries(main ${OGRE_LIBRARIES})
            target_compile_definitions(main PRIVATE HAVE_OGRE3D=1)
            message(STATUS "Ogre3D found via pkg-config")
        else()
            message(WARNING "Ogre3D not found. Ogre3D bindings will be disabled.")
            target_compile_definitions(main PRIVATE HAVE_OGRE3D=0)
        endif()
    else()
        message(WARNING "Ogre3D not found. Ogre3D bindings will be disabled.")
        target_compile_definitions(main PRIVATE HAVE_OGRE3D=0)
    endif()
endif()


target_include_directories(libbu PUBLIC  include src)



if(CMAKE_BUILD_TYPE MATCHES Debug)

if (UNIX)
target_compile_options(main PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g  -D_DEBUG -DVERBOSE)
target_link_options(main PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g  -D_DEBUG) 
endif()
 
   
endif()

target_link_libraries(main libbu)

if (WIN32)
    target_link_libraries(main Winmm.lib)
endif()


if (UNIX)
    target_link_libraries(main  m )
endif()
