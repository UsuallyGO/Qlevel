
Qlevel is just a ShanZhai version of leveldb, it is written by XiaoQ Maxwell. You can get him
in qinliok@126.com.

It must be emphasized that: The writter is one of the poorest programmer in the world, so donot
expect too much for this source code. Perhaps you would soon find the code is so poor...

How to build:
  There are two ways to build the source tar. One is to use the Makefile in the root directory,
the other one is to run the build script in build directory. Cmake toos is needed when build 
with the build script.

******Build with Makefile:
1.cd to your target directory, suppose $TARGET_DIRECTORY=/tmp/level
  cd $TARGET_DIRECTORY
2.run make tool with -f to specify the path of Makefile,suppose $SOURCE_PATH=/home/Qlevel
  make -f $SOURCE_PATH/Makefile

Notice:
1.Make sure Makefile is in the directory of $SOURCE_PATH
2.You can build Qlevel to static library with options BUILD_STATIC=yes
  make -f $SOURCE_PATH/Makefile BUILD_STATIC=yes
  default choose to build shared library
3.You can specify the install path with INSTALL_PREFIX
  make -f $SOURCE_PATH/Makefile INSTALL_PREFIX=/tmp/level
4.You can run make install Qlevel to install the build target(include headers and libqlevel.so or libqlevel.a)
  make -f $SOURCE_PATH/Makefile install

******Build with script of build_config(make sure you have CMAKE tools):
1.cd to your target directory, suppose $TARGET_DIRECTORY=/tmp/level
  cd $TARGET_DIRECTORY
2.run the script of build_config in build directory, suppose $SOURCE_PATH=/hom/Qlevel
  /home/Qlevel/build/build_config

The target and headers will be installed automatically, which is different with the Makefile way.

Notice:
1.You can choose to build Qlevel to static library with options --build_static
  /home/Qlevel/build/build_config --build_static
2.You can choose to specify the install path with options --prefix
  /home/Qlevel/build/build_config --prefix $PWD
  The default install directory is $PWD

****** Pre-built.2 is windows version of pthread. We don't use it right now.
