# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/clion-2019.2.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion-2019.2.2/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/main.cpp.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/main.cpp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main.cpp.dir/flags.make

CMakeFiles/main.cpp.dir/CPU6502.cpp.o: CMakeFiles/main.cpp.dir/flags.make
CMakeFiles/main.cpp.dir/CPU6502.cpp.o: ../CPU6502.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/main.cpp.dir/CPU6502.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.cpp.dir/CPU6502.cpp.o -c /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/CPU6502.cpp

CMakeFiles/main.cpp.dir/CPU6502.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.cpp.dir/CPU6502.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/CPU6502.cpp > CMakeFiles/main.cpp.dir/CPU6502.cpp.i

CMakeFiles/main.cpp.dir/CPU6502.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.cpp.dir/CPU6502.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/CPU6502.cpp -o CMakeFiles/main.cpp.dir/CPU6502.cpp.s

CMakeFiles/main.cpp.dir/CPUBus.cpp.o: CMakeFiles/main.cpp.dir/flags.make
CMakeFiles/main.cpp.dir/CPUBus.cpp.o: ../CPUBus.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/main.cpp.dir/CPUBus.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.cpp.dir/CPUBus.cpp.o -c /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/CPUBus.cpp

CMakeFiles/main.cpp.dir/CPUBus.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.cpp.dir/CPUBus.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/CPUBus.cpp > CMakeFiles/main.cpp.dir/CPUBus.cpp.i

CMakeFiles/main.cpp.dir/CPUBus.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.cpp.dir/CPUBus.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/CPUBus.cpp -o CMakeFiles/main.cpp.dir/CPUBus.cpp.s

CMakeFiles/main.cpp.dir/Emulator.cpp.o: CMakeFiles/main.cpp.dir/flags.make
CMakeFiles/main.cpp.dir/Emulator.cpp.o: ../Emulator.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/main.cpp.dir/Emulator.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.cpp.dir/Emulator.cpp.o -c /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/Emulator.cpp

CMakeFiles/main.cpp.dir/Emulator.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.cpp.dir/Emulator.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/Emulator.cpp > CMakeFiles/main.cpp.dir/Emulator.cpp.i

CMakeFiles/main.cpp.dir/Emulator.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.cpp.dir/Emulator.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/Emulator.cpp -o CMakeFiles/main.cpp.dir/Emulator.cpp.s

CMakeFiles/main.cpp.dir/Memory.cpp.o: CMakeFiles/main.cpp.dir/flags.make
CMakeFiles/main.cpp.dir/Memory.cpp.o: ../Memory.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/main.cpp.dir/Memory.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.cpp.dir/Memory.cpp.o -c /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/Memory.cpp

CMakeFiles/main.cpp.dir/Memory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.cpp.dir/Memory.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/Memory.cpp > CMakeFiles/main.cpp.dir/Memory.cpp.i

CMakeFiles/main.cpp.dir/Memory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.cpp.dir/Memory.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/Memory.cpp -o CMakeFiles/main.cpp.dir/Memory.cpp.s

CMakeFiles/main.cpp.dir/Nesrom.cpp.o: CMakeFiles/main.cpp.dir/flags.make
CMakeFiles/main.cpp.dir/Nesrom.cpp.o: ../Nesrom.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/main.cpp.dir/Nesrom.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.cpp.dir/Nesrom.cpp.o -c /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/Nesrom.cpp

CMakeFiles/main.cpp.dir/Nesrom.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.cpp.dir/Nesrom.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/Nesrom.cpp > CMakeFiles/main.cpp.dir/Nesrom.cpp.i

CMakeFiles/main.cpp.dir/Nesrom.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.cpp.dir/Nesrom.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/Nesrom.cpp -o CMakeFiles/main.cpp.dir/Nesrom.cpp.s

CMakeFiles/main.cpp.dir/PPU2C02.cpp.o: CMakeFiles/main.cpp.dir/flags.make
CMakeFiles/main.cpp.dir/PPU2C02.cpp.o: ../PPU2C02.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/main.cpp.dir/PPU2C02.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.cpp.dir/PPU2C02.cpp.o -c /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/PPU2C02.cpp

CMakeFiles/main.cpp.dir/PPU2C02.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.cpp.dir/PPU2C02.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/PPU2C02.cpp > CMakeFiles/main.cpp.dir/PPU2C02.cpp.i

CMakeFiles/main.cpp.dir/PPU2C02.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.cpp.dir/PPU2C02.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/PPU2C02.cpp -o CMakeFiles/main.cpp.dir/PPU2C02.cpp.s

CMakeFiles/main.cpp.dir/PPUBus.cpp.o: CMakeFiles/main.cpp.dir/flags.make
CMakeFiles/main.cpp.dir/PPUBus.cpp.o: ../PPUBus.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/main.cpp.dir/PPUBus.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.cpp.dir/PPUBus.cpp.o -c /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/PPUBus.cpp

CMakeFiles/main.cpp.dir/PPUBus.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.cpp.dir/PPUBus.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/PPUBus.cpp > CMakeFiles/main.cpp.dir/PPUBus.cpp.i

CMakeFiles/main.cpp.dir/PPUBus.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.cpp.dir/PPUBus.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/PPUBus.cpp -o CMakeFiles/main.cpp.dir/PPUBus.cpp.s

CMakeFiles/main.cpp.dir/helper.cpp.o: CMakeFiles/main.cpp.dir/flags.make
CMakeFiles/main.cpp.dir/helper.cpp.o: ../helper.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/main.cpp.dir/helper.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.cpp.dir/helper.cpp.o -c /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/helper.cpp

CMakeFiles/main.cpp.dir/helper.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.cpp.dir/helper.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/helper.cpp > CMakeFiles/main.cpp.dir/helper.cpp.i

CMakeFiles/main.cpp.dir/helper.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.cpp.dir/helper.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/helper.cpp -o CMakeFiles/main.cpp.dir/helper.cpp.s

CMakeFiles/main.cpp.dir/main.cpp.o: CMakeFiles/main.cpp.dir/flags.make
CMakeFiles/main.cpp.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/main.cpp.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.cpp.dir/main.cpp.o -c /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/main.cpp

CMakeFiles/main.cpp.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.cpp.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/main.cpp > CMakeFiles/main.cpp.dir/main.cpp.i

CMakeFiles/main.cpp.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.cpp.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/main.cpp -o CMakeFiles/main.cpp.dir/main.cpp.s

# Object files for target main.cpp
main_cpp_OBJECTS = \
"CMakeFiles/main.cpp.dir/CPU6502.cpp.o" \
"CMakeFiles/main.cpp.dir/CPUBus.cpp.o" \
"CMakeFiles/main.cpp.dir/Emulator.cpp.o" \
"CMakeFiles/main.cpp.dir/Memory.cpp.o" \
"CMakeFiles/main.cpp.dir/Nesrom.cpp.o" \
"CMakeFiles/main.cpp.dir/PPU2C02.cpp.o" \
"CMakeFiles/main.cpp.dir/PPUBus.cpp.o" \
"CMakeFiles/main.cpp.dir/helper.cpp.o" \
"CMakeFiles/main.cpp.dir/main.cpp.o"

# External object files for target main.cpp
main_cpp_EXTERNAL_OBJECTS =

../bin/main.cpp: CMakeFiles/main.cpp.dir/CPU6502.cpp.o
../bin/main.cpp: CMakeFiles/main.cpp.dir/CPUBus.cpp.o
../bin/main.cpp: CMakeFiles/main.cpp.dir/Emulator.cpp.o
../bin/main.cpp: CMakeFiles/main.cpp.dir/Memory.cpp.o
../bin/main.cpp: CMakeFiles/main.cpp.dir/Nesrom.cpp.o
../bin/main.cpp: CMakeFiles/main.cpp.dir/PPU2C02.cpp.o
../bin/main.cpp: CMakeFiles/main.cpp.dir/PPUBus.cpp.o
../bin/main.cpp: CMakeFiles/main.cpp.dir/helper.cpp.o
../bin/main.cpp: CMakeFiles/main.cpp.dir/main.cpp.o
../bin/main.cpp: CMakeFiles/main.cpp.dir/build.make
../bin/main.cpp: /usr/lib/x86_64-linux-gnu/libglut.so
../bin/main.cpp: /usr/lib/x86_64-linux-gnu/libXmu.so
../bin/main.cpp: /usr/lib/x86_64-linux-gnu/libXi.so
../bin/main.cpp: CMakeFiles/main.cpp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking CXX executable ../bin/main.cpp"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main.cpp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main.cpp.dir/build: ../bin/main.cpp

.PHONY : CMakeFiles/main.cpp.dir/build

CMakeFiles/main.cpp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main.cpp.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main.cpp.dir/clean

CMakeFiles/main.cpp.dir/depend:
	cd /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug /home/wyattpochman/Documents/Gabes_Stuff/nesemu/program/cmake-build-debug/CMakeFiles/main.cpp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/main.cpp.dir/depend
