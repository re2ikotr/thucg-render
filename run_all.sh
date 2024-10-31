#!/usr/bin/env bash

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    echo "good"
else
    rm -rf build
fi
mkdir -p build
cd build
cmake ..
make -j
cd ..

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output
# bin/PA1 testcases/scene01_basic.txt output/scene01.bmp
# bin/PA1 testcases/scene02_cube.txt output/scene02.bmp
# bin/PA1 testcases/scene03_sphere.txt output/scene03.bmp
# bin/PA1 testcases/scene04_axes.txt output/scene04.bmp
# bin/PA1 testcases/scene05_bunny_200.txt output/scene05.bmp
# bin/PA1 testcases/scene06_bunny_1k.txt output/scene06.bmp
# bin/PA1 testcases/scene07_shine.txt output/scene07.bmp
# bin/PA1 testcases/scene08_test.txt output/scene11.bmp > test.log
# bin/PA1 testcases/scene09_ball.txt output/scene14.bmp > test.log
# bin/PA1 testcases/bunny_test.txt output/scene12_bunny.bmp > test.log
# bin/PA1 testcases/scene13_cornell_test.txt output/scene13_cornell_test.bmp > test.log
# bin/PA1 testcases/bezier_test.txt output/scene14_bezier_test.bmp > test.log
# bin/PA1 testcases/scene15_moving_test.txt output/scene23.bmp > test.log
# bin/PA1 testcases/scene16_angel_lucy.txt output/scene16.bmp > test.log
# bin/PA1 testcases/scene17_diamond.txt output/scene17.bmp > test.log
# bin/PA1 testcases/scene18_bunny_200.txt output/scene18.bmp > test.log
# bin/PA1 testcases/scene19_bunny_sppm.txt output/scene19.bmp > test.log
bin/PA1 testcases/scene20_moving.txt output/scene20.bmp > test.log
# bin/PA1 testcases/scene21_bezier.txt output/scene21.bmp > test.log
# bin/PA1 testcases/bezier_test.txt output/scene22_bezier_test.bmp > test.log
# bin/PA1 testcases/scene19_bunny_sppm.txt output/scene24.bmp > test.log
# bin/PA1 testcases/final.txt output/scene25_final.bmp > test.log
