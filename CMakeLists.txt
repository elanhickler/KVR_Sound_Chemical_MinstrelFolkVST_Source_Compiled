cmake_minimum_required(VERSION 3.22)
project(MinstrelFolkVST VERSION 1.0.0 LANGUAGES C CXX)

# Pass -DJUCE_SOURCE_DIR=... to a JUCE 8 tree, or install JUCE and use find_package.
if(NOT JUCE_SOURCE_DIR AND DEFINED ENV{JUCE_DIR})
    set(JUCE_SOURCE_DIR "$ENV{JUCE_DIR}")
endif()
if(JUCE_SOURCE_DIR)
    if(NOT EXISTS "${JUCE_SOURCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "JUCE not found at ${JUCE_SOURCE_DIR}")
    endif()
    add_subdirectory("${JUCE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/JUCE" EXCLUDE_FROM_ALL)
else()
    find_package(JUCE CONFIG REQUIRED)
endif()

juce_add_plugin(MinstrelFolk
    COMPANY_NAME "Open Folk Instruments"
    BUNDLE_ID "audio.openfolk.minstrelfolk"
    PLUGIN_MANUFACTURER_CODE Ofki
    PLUGIN_CODE MnFk
    FORMATS VST3 Standalone
    PRODUCT_NAME "Minstrel Folk"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(MinstrelFolk)

target_sources(MinstrelFolk PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_features(MinstrelFolk PRIVATE cxx_std_17)
target_compile_definitions(MinstrelFolk PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(MinstrelFolk PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
)
