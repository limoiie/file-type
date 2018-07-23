cmake_minimum_required(VERSION 3.10)
project(file_type)

set(CMAKE_CXX_STANDARD 14)

add_executable(file_type main.cpp
        CMagicSet.cpp CMagicSet.h
        CFileChecker.cpp CFileChecker.h
        CMagicSetHelper.cpp CMagicSetHelper.h
        CFileType.cpp CFileType.h
        encodings.h encodings.cpp test/test-encoding.cpp)