project(ECS)

set(SRC_FILES
    main.cpp
)

set(H_FILES
    Component.hpp
    Entity.hpp
    EntityManager.hpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${H_FILES})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
