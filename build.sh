#!/bin/sh

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
    echo "Creating html folder"
    mkdir ./html/css
fi


# check  if need to create html/js folder
if [ -d "./html/js" ]
then
    echo "Directory html/js exists."
else
    echo "Creating html folder"
    mkdir ./html/js
fi


# check  if need to create html/fonts folder
if [ -d "./html/fonts" ]
then
    echo "Directory html/fonts exists."
else
    echo "Creating html folder"
    mkdir ./html/fonts
fi

# remove old windows app folder
if [ -d "./Configurator-win32-x64" ]
then
    echo "clean up old version of windows app"
    rm -r ./Configurator-win32-x64
else
    echo "no old version of windows app to clean up"
fi

echo "minifying html5 app files"
node build.js

echo "build windows app"
electron-packager C:/Users/crumb/Desktop/desktop Configurator --platform=win32 --arch=x64
echo "build complete"
