cmake_minimum_required(VERSION 3.16)

project(CanGuiApp LANGUAGES CXX)

# Qt paketlerini bul
find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#==================================================================
# 1. ADIM: SADECE TEMEL GUI DOSYALARIYLA HEDEFİ OLUŞTUR
#==================================================================
add_executable(CanGuiApp
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

#==================================================================
# 2. ADIM: DİĞER TÜM KAYNAK VE BAŞLIK DOSYALARINI HEDEFE EKLE
#==================================================================

set(PROJECT_FILES
    # Configuration Dosyaları
    ${CMAKE_CURRENT_SOURCE_DIR}/../configuration/src/configuration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../configuration/include/configuration.hpp

    # Receiver Dosyaları
    ${CMAKE_CURRENT_SOURCE_DIR}/../receiver/src/receive.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../receiver/include/receive.hpp

    # Transmitter Dosyaları
    ${CMAKE_CURRENT_SOURCE_DIR}/../transmitter/src/transmit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../transmitter/include/transmit.hpp
)

target_sources(CanGuiApp PRIVATE
     ${PROJECT_FILES}
)

#==================================================================
# 3. ADIM: GEREKLİ INCLUDE VE KÜTÜPHANE YOLLARINI HEDEFE BAĞLA
#==================================================================
target_include_directories(CanGuiApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../configuration/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../receiver/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../transmitter/include
)

target_link_directories(CanGuiApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../driver/lib
)

target_link_libraries(CanGuiApp PRIVATE
    Qt6::Widgets
    pcanfd
)
