set(CUE_ALL_OPTIONS
    CUE_WEBP
    CUE_GIF
)

option(CUE_WEBP "Enable WEBP sprites" OFF)
option(CUE_GIF "Enable GIF sprites" OFF)

option(CUE_ENABLE_ALL "Enable ALL cue features" OFF)

if (CUE_ENABLE_ALL)
    foreach (opt IN LISTS CUE_ALL_OPTIONS)
        set(${opt} ON CACHE BOOL "Forced by CUE_ENABLE_ALL" FORCE)
    endforeach()
endif()

foreach(opt in LISTS CUE_ALL_OPTIONS)
    if (${opt})
        add_compile_definitions(${opt}=1)
    endif()
endforeach()

# Print all enabled features

set(CUE_ENABLED_OPTIONS "core")

if (CUE_WEBP)
    list(APPEND CUE_ENABLED_OPTIONS "webp")
endif()
if (CUE_GIF)
    list(APPEND CUE_ENABLED_OPTIONS "gif")
endif()

# convert to string
list(JOIN CUE_ENABLED_OPTIONS ", " CUE_ENABLED_OPTIONS_STR)

message(STATUS "Enabled cue features: ${CUE_ENABLED_OPTIONS_STR}")
