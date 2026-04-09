cmake_minimum_required(VERSION 3.22)

project(UtilityFilters VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.6
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(UtilityFilters
  COMPANY_NAME "Codex"
  BUNDLE_ID "com.codex.utilityfilters"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
  PLUGIN_MANUFACTURER_CODE CdEx
  PLUGIN_CODE UtFl
  FORMATS AU VST3 Standalone
  PRODUCT_NAME "Utility Filters"
)

juce_generate_juce_header(UtilityFilters)

target_sources(UtilityFilters
  PRIVATE
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
)

target_compile_definitions(UtilityFilters
  PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_link_libraries(UtilityFilters
  PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_basics
    juce::juce_gui_extra
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
