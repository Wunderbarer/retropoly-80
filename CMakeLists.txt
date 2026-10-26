cmake_minimum_required(VERSION 3.22)
project(RetroPoly80 VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE wird als Unterordner erwartet:  git submodule add https://github.com/juce-framework/JUCE
add_subdirectory(JUCE)

juce_add_plugin(RetroPoly80
    COMPANY_NAME "DeinStudioName"
    PRODUCT_NAME "RetroPoly-80"
    PLUGIN_MANUFACTURER_CODE Dstu
    PLUGIN_CODE Rp80
    FORMATS VST3 Standalone
    VST3_CATEGORIES Synth Instrument
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    PRODUCES_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(RetroPoly80 PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Presets/PresetManager.cpp
    Source/GUI/PresetBar.cpp
)

juce_add_binary_data(RetroPoly80Presets SOURCES
    # optional: hier koennten .xml Faktory-Presets als BinaryData eingebunden werden
)

target_compile_definitions(RetroPoly80 PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(RetroPoly80 PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_audio_processors
    juce::juce_gui_basics
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
