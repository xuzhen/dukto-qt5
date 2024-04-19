# Dukto

Dukto is an easy file transfer tool for LAN. It was created by Emanuele Colombo, and ported to Qt 5/6 by me and [other contributors](https://github.com/xuzhen/dukto-qt5/graphs/contributors).

Now it supports Windows, Linux, MacOS and Android.

## Warning
Dukto transfers files and text without encryption and is only designed for use in trusted network environments.

### Prebuilt Packages

#### Windows
Portable versions can be downloaded from [the releases page](https://github.com/xuzhen/dukto-qt5/releases)

The Qt6 version supports Windows 10+ only. If you are still using Windows 7, download the Qt5 version instead.

If you can not open the 7z files, visit https://7-zip.org/ and install 7-zip

If you get `The program can't start because MSVCP140.dll is missing from your computer. Try reinstalling the program to fix this problem` error , download and install the Visual C++ Redistributable packages for VS2015-2022 from [Microsoft](https://learn.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist#visual-studio-2015-2017-2019-and-2022). 
Direct links: [X64](https://aka.ms/vs/17/release/vc_redist.x64.exe) or [X86](https://aka.ms/vs/17/release/vc_redist.x86.exe)


#### Android
APKs can be downloaded from [the releases page](https://github.com/xuzhen/dukto-qt5/releases)

The `dukto_*_qt6.apk` supports Android 8.0 (Oreo) and later.

The `dukto_*_qt5.apk` supports Android 5.0 (Lollipop) and later.

#### Ubuntu and derivatives:
Use [this PPA](https://launchpad.net/~xuzhen666/+archive/ubuntu/dukto) 

### Build from source code


#### Build Dependencies

* Qt 5.3+
* libnotify (optional, Linux only)
* Android SDK and NDK (Android only)

#### For Windows, Linux, MacOS

Run the following command in the source code directory to build:

* QMake 
```sh
mkdir build && cd build && qmake .. && make
```

* CMake 
```sh
mkdir build && cd build && cmake .. && make
```

#### For Android

* Build with Qt6:
```sh
mkdir build && cd build
/path/to/qt6/bin/qt-cmake -DANDROID_NDK_ROOT=/path/to/ndk -DANDROID_SDK_ROOT=/path/to/sdk ..
make
```

* Build with Qt5:
```sh
mkdir build && cd build
export ANDROID_NDK_ROOT=/path/to/ndk ANDROID_SDK_ROOT=/path/to/sdk
cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a -DQT_CMAKE_ROOT=/path/to/qt/cmake ..
make
```
