project(ECS)

set(SRC_FILES
    main.cpp
    ComponentStore.cpp
)

set(H_FILES
    EntityManager.hpp
    ComponentStore.hpp
    Defines.hpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${H_FILES})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
