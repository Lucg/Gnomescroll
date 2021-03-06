#!/usr/bin/env bash

if [ -z "$1" ]; then
    arch="32"
else
    arch="$1"
fi

if [ $arch != "32" ] && [ $arch != "64" ]; then
    echo "Unknown architecture:" $arch
    exit 1
fi

VERSION_PATH=../src/c_lib/common/version.h
if [ ! -e ${VERSION_PATH} ]; then
    echo "version.h not found at: " ${VERSION_PATH}
    exit 1
fi

version=`cat ../src/c_lib/common/version.h | grep GS_VERSION | cut -d " " -f 3`
if [[ "$version" == */* ]]; then
    echo "Invalid version:" $version
    exit 1
fi
if [ -z "$version" ]; then
    echo "Invalid version:" $version
    exit 1
fi

./waf configure --release=production --arch=$arch
./waf --nocache

f="gnomescroll_linux"$arch"_"$version

if [ -d "$f" ]; then
    rm -rf $f
fi

mkdir "$f"
cp -d run "$f"/gnomescroll
cp -d -R media "$f"/
mkdir "$f"/settings
cp -d ./settings/production.lua "$f"/settings/settings.lua
cp -d ./settings/lua_library.lua "$f"/settings/lua_library.lua
cp -d ./settings/load_options.lua "$f"/settings/load_options.lua
mkdir "$f"/screenshot

mkdir -p "$f"/lib/lin$arch/
cp -d ../lib/lin$arch/*.so "$f"/lib/lin$arch/
cp -d ../lib/lin$arch/*.so.* "$f"/lib/lin$arch/
#cp -R -d ../lib/lin$arch/awesomium "$f"/lib/lin$arch/

# Remove any existing package
rm "$f".tar.gz
# Create package
tar cvzf "$f".tar.gz "$f"

# Remove any existing local saved copy of the folder
mkdir -p ~/gs_build/
rm -rf ~/gs_build/"$f"
# Move the folder to our stash
mv "$f" ~/gs_build/

# copy the package to the server
ssh root@direct.gnomescroll.com 'mkdir /var/www/gnomescroll_downloads/'${version}'/'
scp "$f".tar.gz root@direct.gnomescroll.com:/var/www/gnomescroll_downloads/"$version"/"$f".tar.gz

rm "$f".tar.gz
