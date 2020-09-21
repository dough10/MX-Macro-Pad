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
node build.js

echo "build windows app"
npm run package-win64

echo "build linux app"
npm run package-linux64

echo "build mac app"
npm run package-mac64


echo "build complete"
