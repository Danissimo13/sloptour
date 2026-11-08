sloptour_embed_assets(sloptour_embedded_assets "${PROJECT_SOURCE_DIR}/assets")

sloptour_add_layer(sloptour_infrastructure
    DEPENDENCIES
        sloptour_application
        nlohmann_json::nlohmann_json
    PRIVATE_DEPENDENCIES
        sloptour_embedded_assets
        reproc::reproc
)
