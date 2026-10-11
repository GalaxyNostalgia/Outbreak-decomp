# Biohazard Outbreak PC port.
#
#   cmake -S pc -B build/pc -DOUTBREAK_GAME=f1      (32-bit toolchain required, see docs/PCPORT.md)
#   cmake --build build/pc
#   build/pc/outbreak-f1 --test "Bio Hazard Outbreak - File 1.iso"
cmake_minimum_required(VERSION 3.16)
project(outbreak_pc C)

set(OUTBREAK_GAME "f1" CACHE STRING "f1 (Biohazard Outbreak) or f2 (File 2)")
if(OUTBREAK_GAME STREQUAL "f1")
  set(GAME_ELF "SLPM_654.28")
elseif(OUTBREAK_GAME STREQUAL "f2")
  set(GAME_ELF "SLPM_656.92")
else()
  message(FATAL_ERROR "OUTBREAK_GAME must be f1 or f2")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(FATAL_ERROR "The PC port must be built as 32-bit (pointers are 4 bytes on the PS2). "
                      "Linux: -DCMAKE_C_FLAGS=-m32. Windows: an i686 MinGW-w64 toolchain.")
endif()

set(REPO ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(GEN ${CMAKE_CURRENT_BINARY_DIR}/gen)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS ${REPO}/src/${GAME_ELF}/*.c)

add_custom_command(
  OUTPUT ${GEN}/stubs.c ${GEN}/data_symbols.ld
  COMMAND ${Python3_EXECUTABLE} -I ${CMAKE_CURRENT_SOURCE_DIR}/tools/gen_link.py ${GAME_ELF} ${GEN}
  DEPENDS ${GAME_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/tools/gen_link.py
  COMMENT "Generating stubs and data symbols for ${GAME_ELF}")

# Decompiled game code: the same C files the PS2 build matches against
add_library(game OBJECT ${GAME_SOURCES})
target_include_directories(game PRIVATE ${REPO}/include)
target_compile_definitions(game PRIVATE PLATFORM_PC VERSION_${OUTBREAK_GAME})
string(TOUPPER "VERSION_${OUTBREAK_GAME}" VERSION_DEF)
target_compile_definitions(game PRIVATE ${VERSION_DEF})
target_compile_options(game PRIVATE -fno-builtin -fno-strict-aliasing -fwrapv -w)

add_executable(outbreak-${OUTBREAK_GAME}
  src/main.c src/guest.c src/iso9660.c src/loader.c src/sce_io.c src/libc_extra.c src/log.c
  ${GEN}/stubs.c $<TARGET_OBJECTS:game>)
target_include_directories(outbreak-${OUTBREAK_GAME} PRIVATE src)
target_compile_definitions(outbreak-${OUTBREAK_GAME} PRIVATE GAME_ELF="${GAME_ELF}" _FILE_OFFSET_BITS=64)
target_compile_options(outbreak-${OUTBREAK_GAME} PRIVATE -Wall -Wextra -Wno-unused-parameter)
# PS2 data symbols are absolute addresses inside guest memory
target_link_options(outbreak-${OUTBREAK_GAME} PRIVATE -Wl,${GEN}/data_symbols.ld -no-pie)
if(WIN32)
  # keep the program image out of the PS2 address range (0x00100000-0x02000000)
  target_link_options(outbreak-${OUTBREAK_GAME} PRIVATE -Wl,--image-base,0x40000000)
endif()
