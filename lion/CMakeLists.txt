cmake_minimum_required(VERSION 3.16)

project(LionJump VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 为MSVC添加C++17支持标志
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
        main.cpp



)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(LionJump
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        Pic.qrc

        menu.h menu.cpp menu.ui
        LevelSelect.h LevelSelect.cpp
        GameSettings.h
        SettingsPage.h SettingsPage.cpp


        GameScene.h
        GameScene.cpp
        LionAnimation.h
        LionAnimation.cpp
        player.h
        palyer.cpp
        Config.h
        
        # 新增：关卡系统文件
        LevelData.h
        LevelData.cpp
        LevelManager.h
        LevelManager.cpp
        
        # 新增：关卡编辑器文件
        LevelEditor.h
        LevelEditor.cpp
        AudioController.h
        AudioController.cpp
        
        # 新增：启动画面和帮助页面
        SplashScreen.h
        SplashScreen.cpp
        HelpPage.h
        HelpPage.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET LionJump APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(LionJump SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(LionJump
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(LionJump PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.LionJump)
endif()
set_target_properties(LionJump PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS LionJump
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LionJump)
endif()
