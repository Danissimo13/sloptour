function(sloptour_collect_sources output directory)
    file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        "${directory}/*.cpp"
        "${directory}/*.hpp"
    )
    set(${output} ${sources} PARENT_SCOPE)
endfunction()

function(sloptour_record_layer target)
    cmake_parse_arguments(RECORD "" "" "DEPENDENCIES" ${ARGN})
    file(RELATIVE_PATH folder "${PROJECT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}")
    set(layer_dependencies "")
    foreach(dependency IN LISTS RECORD_DEPENDENCIES)
        if(dependency MATCHES "^sloptour_")
            list(APPEND layer_dependencies "${dependency}")
        endif()
    endforeach()
    set_property(GLOBAL APPEND PROPERTY SLOPTOUR_LAYERS "${target}")
    set_property(GLOBAL PROPERTY "SLOPTOUR_LAYER_FOLDER_${target}" "${folder}")
    set_property(GLOBAL PROPERTY "SLOPTOUR_LAYER_DEPENDENCIES_${target}" "${layer_dependencies}")
endfunction()

function(sloptour_layer_reach target output)
    set(reached "")
    set(pending "${target}")
    while(pending)
        list(POP_FRONT pending current)
        if(current IN_LIST reached)
            continue()
        endif()
        list(APPEND reached "${current}")
        get_property(dependencies GLOBAL PROPERTY "SLOPTOUR_LAYER_DEPENDENCIES_${current}")
        list(APPEND pending ${dependencies})
    endwhile()
    set(${output} "${reached}" PARENT_SCOPE)
endfunction()

function(sloptour_add_layer_rules_test)
    get_property(layers GLOBAL PROPERTY SLOPTOUR_LAYERS)
    set(rules "set(SLOPTOUR_LAYERS \"${layers}\")\n")
    foreach(layer IN LISTS layers)
        get_property(folder GLOBAL PROPERTY "SLOPTOUR_LAYER_FOLDER_${layer}")
        sloptour_layer_reach(${layer} reached)
        set(allowed "")
        foreach(reachable IN LISTS reached)
            get_property(reachable_folder GLOBAL PROPERTY "SLOPTOUR_LAYER_FOLDER_${reachable}")
            list(APPEND allowed "${reachable_folder}")
        endforeach()
        string(APPEND rules "set(SLOPTOUR_LAYER_FOLDER_${layer} \"${folder}\")\n")
        string(APPEND rules "set(SLOPTOUR_LAYER_EXCLUDED_${layer} \"\")\n")
        string(APPEND rules "set(SLOPTOUR_LAYER_ALLOWED_${layer} \"${allowed}\")\n")
    endforeach()
    set(rules_file "${PROJECT_BINARY_DIR}/LayerRules.cmake")
    file(WRITE "${rules_file}" "${rules}")
    add_test(NAME layer_dependencies
        COMMAND "${CMAKE_COMMAND}" "-DRULES=${rules_file}" "-DSOURCE_DIR=${PROJECT_SOURCE_DIR}/src"
                -P "${PROJECT_SOURCE_DIR}/cmake/CheckLayerIncludes.cmake")
endfunction()

function(sloptour_add_layer target)
    cmake_parse_arguments(LAYER "" "" "DEPENDENCIES;PRIVATE_DEPENDENCIES" ${ARGN})

    sloptour_record_layer(${target} DEPENDENCIES ${LAYER_DEPENDENCIES} ${LAYER_PRIVATE_DEPENDENCIES})
    sloptour_collect_sources(sources "${CMAKE_CURRENT_SOURCE_DIR}")
    add_library(${target} STATIC ${sources})

    target_include_directories(${target} PUBLIC "${PROJECT_SOURCE_DIR}/src")
    target_link_libraries(${target}
        PUBLIC ${LAYER_DEPENDENCIES}
        PRIVATE ${LAYER_PRIVATE_DEPENDENCIES} sloptour_warnings
    )
    target_compile_features(${target} PUBLIC cxx_std_23)

    sloptour_enable_clang_tidy(${target})
endfunction()

function(sloptour_add_executable target)
    cmake_parse_arguments(EXECUTABLE "" "OUTPUT_NAME" "DEPENDENCIES" ${ARGN})

    sloptour_collect_sources(sources "${CMAKE_CURRENT_SOURCE_DIR}")
    add_executable(${target} ${sources})
    if(EXECUTABLE_OUTPUT_NAME)
        set_target_properties(${target} PROPERTIES OUTPUT_NAME "${EXECUTABLE_OUTPUT_NAME}")
    endif()

    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/src")
    target_link_libraries(${target} PRIVATE ${EXECUTABLE_DEPENDENCIES} sloptour_warnings)

    sloptour_enable_clang_tidy(${target})
endfunction()

function(sloptour_enable_clang_tidy target)
    if(SLOPTOUR_ENABLE_CLANG_TIDY)
        set_target_properties(${target} PROPERTIES CXX_CLANG_TIDY "${SLOPTOUR_CLANG_TIDY_COMMAND}")
    endif()
endfunction()

function(sloptour_embed_assets target directory)
    file(GLOB_RECURSE assets CONFIGURE_DEPENDS "${directory}/*")
    set(generated "${CMAKE_CURRENT_BINARY_DIR}/EmbeddedAssets.cpp")
    add_custom_command(
        OUTPUT "${generated}"
        COMMAND "${CMAKE_COMMAND}" "-DASSETS_DIRECTORY=${directory}" "-DOUTPUT=${generated}"
                -P "${PROJECT_SOURCE_DIR}/cmake/EmbedAssets.cmake"
        DEPENDS ${assets} "${PROJECT_SOURCE_DIR}/cmake/EmbedAssets.cmake"
        COMMENT "Embedding assets"
        VERBATIM
    )
    add_library(${target} STATIC "${generated}")
    target_include_directories(${target} PUBLIC "${PROJECT_SOURCE_DIR}/src")
    target_compile_features(${target} PUBLIC cxx_std_23)
endfunction()
