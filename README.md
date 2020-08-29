# QuickStart
This is a simple binary data receiver and storage program. Except for supporting
 receiving binary data into file, this program can also support converting the 
 binary file to csv file.

1. Prerequisites  

    This program need a C++11-compliant compiler to build.

2. Building  

    Create a build directory, and change current working directory into build 
     directory, and then execute cmake command to create a solution.

    For example, to build a Visual Studio 2019 edition, run following command in
     cmd prompt:

        cmake -G "Visual Studio 16 2019" ..

    And then run following command to build in cmd prompt:
        cmake --build . --config Release --target install
