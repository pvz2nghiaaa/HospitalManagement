cmake_minimum_required(VERSION 3.16)

project(HospitalManagement VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
    QT NAMES Qt6 Qt5
    REQUIRED COMPONENTS
    Widgets
    Sql
    PrintSupport
)

find_package(
    Qt${QT_VERSION_MAJOR}
    REQUIRED COMPONENTS
    Widgets
    Sql
    PrintSupport
)

set(PROJECT_SOURCES
    src/main.cpp
    
    # Models
    src/models/user.h
    src/models/user.cpp
    src/models/admin.h
    src/models/admin.cpp
    src/models/doctor.h
    src/models/doctor.cpp
    src/models/receptionist.h
    src/models/receptionist.cpp
    src/models/patient.h
    src/models/medicalrecord.h
    src/models/medicalrecord.cpp
    src/models/diagnosis.h
    src/models/diagnosis.cpp
    src/models/prescription.h
    src/models/prescription.cpp
    src/models/prescriptiondetail.h
    src/models/drug.h
    src/models/drug.cpp
    src/models/invoice.h
    src/models/invoice.cpp
    src/models/billableitem.h
    src/models/billableitem.cpp
    src/models/attendancelog.h
    src/models/attendancelog.cpp
    src/models/permission.h
    src/models/permission.cpp

    # Database Managers
    src/database/billingmanager.h
    src/database/billingmanager.cpp

    # Widgets
    src/widgets/activitychartwidget.h
    src/widgets/activitychartwidget.cpp

    # UI Windows & Controllers
    src/ui/login/loginwindow.h
    src/ui/login/loginwindow.cpp
    src/ui/login/LoginWindow.ui

    src/ui/admin/adminwindow.h
    src/ui/admin/adminwindow.cpp
    src/ui/admin/AdminWindow.ui

    src/ui/doctor/doctorwindow.h
    src/ui/doctor/doctorwindow.cpp
    src/ui/doctor/DoctorWindow.ui
    src/ui/doctor/doctor_clinical.cpp
    src/ui/doctor/doctor_patient.cpp
    src/ui/doctor/doctor_profile.cpp
    src/ui/doctor/doctor_records.cpp

    src/ui/receptionist/receptionistwindow.h
    src/ui/receptionist/receptionistwindow.cpp
    src/ui/receptionist/ReceptionistWindow.ui

    # Resources & Assets
    resources/asset/img.qrc
)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(HospitalManagement
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(HospitalManagement SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(HospitalManagement
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(
    HospitalManagement
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::PrintSupport
)

add_custom_command(TARGET HospitalManagement POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    "$<TARGET_FILE_DIR:HospitalManagement>/resources"
)

# Tell the compiler to look in all source subfolders so that flat includes (e.g. #include "user.h") still resolve correctly.
target_include_directories(HospitalManagement PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/database
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/admin
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/doctor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/login
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/receptionist
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.HospitalManagement)
endif()
set_target_properties(HospitalManagement PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS HospitalManagement
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(HospitalManagement)
endif()
