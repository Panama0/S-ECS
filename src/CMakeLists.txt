project(ECS)

set(SRC_FILES
    ComponentStore.cpp
)

set(H_FILES
    EntityManager.hpp
    ComponentStore.hpp
    Defines.hpp
)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${H_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC .)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
