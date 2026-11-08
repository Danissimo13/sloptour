file(GLOB_RECURSE assets RELATIVE "${ASSETS_DIRECTORY}" "${ASSETS_DIRECTORY}/*")
list(SORT assets)
list(LENGTH assets count)

set(CHUNK_BYTES 2000)
math(EXPR CHUNK_HEX "${CHUNK_BYTES} * 2")

set(entries "")
foreach(asset IN LISTS assets)
    file(READ "${ASSETS_DIRECTORY}/${asset}" hex HEX)
    string(LENGTH "${hex}" hex_length)
    set(literal "")
    set(offset 0)
    while(offset LESS hex_length)
        string(SUBSTRING "${hex}" ${offset} ${CHUNK_HEX} chunk)
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "\\\\x\\1" escaped "${chunk}")
        string(APPEND literal "\n        \"${escaped}\"")
        math(EXPR offset "${offset} + ${CHUNK_HEX}")
    endwhile()
    if(literal STREQUAL "")
        set(literal "\"\"")
    endif()
    math(EXPR size "${hex_length} / 2")
    string(APPEND entries "    EmbeddedAsset{.path = \"${asset}\", .content = std::string_view(${literal}, ${size})},\n")
endforeach()

file(WRITE "${OUTPUT}.tmp"
"#include \"Infrastructure/Assets/EmbeddedAssets.hpp\"\n\n#include <array>\n#include <string_view>\n\n"
"namespace sloptour::infrastructure {\n\nnamespace {\n\n"
"constexpr std::array<EmbeddedAsset, ${count}> ASSETS{\n${entries}};\n\n}\n\n"
"std::span<const EmbeddedAsset> embedded_assets() noexcept {\n    return ASSETS;\n}\n\n}\n")
file(COPY_FILE "${OUTPUT}.tmp" "${OUTPUT}" ONLY_IF_DIFFERENT)
file(REMOVE "${OUTPUT}.tmp")
