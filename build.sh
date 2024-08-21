#!/bin/sh
###
 # @Description: 
 # @Author: vincent_xjw@163.com
 # @Date: 2023-03-03 10:30:07
 # @LastEditTime: 2023-03-03 18:10:40
 # @LastEditors: vincent_xjw@163.com
### 
if [ ! -d "build" ]; then
    echo "not exist build dir, create it."
    mkdir -p build
else
    echo "exist build dir."
fi

# cmake -S . -B ./build
# cd ./build
# make -j8
# cd ..

cd build
echo `pwd`
cmake ../CMakeLists.txt --build ./ --open ../ -DCMAKE_BUILD_TYPE=Release
make -j8 && make install && cd ../