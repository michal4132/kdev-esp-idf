add_definitions(-DTRANSLATION_DOMAIN=\"esp-idf\")

include_directories(.)

set(esp-idf_UIS
    flash_config.ui
)

set(esp-idf_SRCS
    esp-idf.cpp
    flashjob.cpp
)


ecm_qt_declare_logging_category(esp-idf_SRCS
    HEADER debug.h
    IDENTIFIER PLUGIN_ESP_IDF
    CATEGORY_NAME "kdevelop.plugins.esp-idf"
    DESCRIPTION "KDevelop plugin: esp-idf"
    EXPORT ESP_IDF
)

ki18n_wrap_ui(esp-idf_SRCS ${esp-idf_UIS})


kdevplatform_add_plugin(esp-idf
    SOURCES ${esp-idf_SRCS}
)



target_link_libraries(esp-idf
    KDev::Interfaces
    KDev::Util
    KDev::Project
    KDev::OutputView
    KF5::I18n
)
