cmake_minimum_required(VERSION 3.14)

project(SwarmEngine)
set(CMAKE_CXX_STANDARD 17)

# --- FIX DLA NOWEGO CMAKE (BŁĄD WERSJI) ---
# To jest odpowiedź na błąd, który dostałeś.
# Wymuszamy na CMake, aby przy podprojektach (libzmq) nie panikował przy starych wersjach.
set(CMAKE_POLICY_VERSION_MINIMUM "3.5")

# ------------------------------------------
include(FetchContent)

# --- KONFIGURACJA ZALEŻNOŚCI ---
set(ZMQ_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CPPZMQ_BUILD_TESTS OFF CACHE BOOL "" FORCE)

# Rdzeń (libzmq)
FetchContent_Declare(
    libzmq
    GIT_REPOSITORY https://github.com/zeromq/libzmq.git
    GIT_TAG v4.3.5
)

# Nakładka C++ (cppzmq)
FetchContent_Declare(
    cppzmq
    GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
    GIT_TAG v4.10.0
)

# Ładujemy
FetchContent_MakeAvailable(libzmq cppzmq)

# -------------------------------
add_executable(SwarmEngine main.cpp)

# Linkowanie (bez zmian)
target_link_libraries(SwarmEngine PRIVATE cppzmq libzmq-static)