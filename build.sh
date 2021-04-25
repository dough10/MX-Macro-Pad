#!/bin/sh
PS3="Choose keymap: "
maps=("Openauto pro" "Default" "Quit")
select m in "${maps[@]}"; do
    case $m in 
        "Openauto pro")
            echo "Using Openauto pro keymap"
            cp "for car pi"/keys.json src/keys.json
            break
            ;;
        "Default")
            echo "Using Default keymap"
            cp "default keys"/keys.json src/keys.json
            break
            ;;
        "Quit")
            echo "Exiting"
            exit
            break
            ;;
    esac
done

# check  if need to create html folder
if [ -d "./html" ]
then
    echo "Directory html exists."
else
    echo "Creating html folder"
    mkdir ./html
fi


# check  if need to create html/css folder
if [ -d "./html/css" ]
then
    echo "Directory html/css exists."
else
    echo "Creating html/css folder"
    mkdir ./html/css
fi


# check  if need to create html/js folder
if [ -d "./html/js" ]
then
    echo "Directory html/js exists."
else
    echo "Creating html/js folder"
    mkdir ./html/js
fi


# check  if need to create html/fonts folder
if [ -d "./html/fonts" ]
then
    echo "Directory html/fonts exists."
else
    echo "Creating html/fonts folder"
    mkdir ./html/fonts
fi
        

echo "minifying html5 app files"
node modules/build.js

echo "Application build complete"

PS3="Select OS Package: "
maps=("Windows" "ARM" "Quit")
select m in "${maps[@]}"; do
    case $m in 
        "Windows")
            echo "Creating Windows build"
            electron-zip-packager . Configurator --overwrite --asar --platform=win32 --arch=x64 --prune=true --out=release-builds
            break
            ;;
        "Default")
            echo "Creating ARM build"
            electron-zip-packager . Configurator --overwrite --asar --platform=linux --arch=armv7l --prune=true --out=release-builds
            break
            ;;
        "Quit")
            echo "Exiting"
            exit
            break
            ;;
    esac
done

echo "Application build complete"