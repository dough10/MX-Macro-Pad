#!/bin/sh

CODE='\033['

black$CODE'0;30m'
RED=$CODE'0;31m'
GREEN=$CODE'0;32m'
ORANGE=$CODE'0;33m'
BLUE=$CODE'0;34m'
PURPLE=$CODE'0;35m'
CYAN=$CODE'0;36m'
GREY=$CODE'0;37m'
YELLOW=$CODE'1;33m'
WHITE=$CODE'1;37m'
NC=$CODE'0m'

PS3="Choose keymap: "
maps=("Openauto pro" "Default" "Quit")
select m in "${maps[@]}"; do
    case $m in 
        "Openauto pro")
            echo -e "Using ${BLUE}Openauto pro${NC} keymap"
            cp "for car pi"/keys.json src/keys.json
            break
            ;;
        "Default")
            echo -e "Using ${BLUE}Default${NC} keymap"
            cp "default keys"/keys.json src/keys.json
            break
            ;;
        "Quit")
            echo -e "Exiting"
            exit
            break
            ;;
        *) echo "invalid option $REPLY";;
    esac
done

# check  if need to create html folder
if [ -d "./html" ]
then
    echo -e "Directory ${GREEN}html${NC} exists."
else
    echo -e "Creating ${GREEN}html${NC} folder"
    mkdir ./html
fi


# check  if need to create html/css folder
if [ -d "./html/css" ]
then
    echo -e "Directory ${GREEN}html/css${NC} exists."
else
    echo -e "Creating ${GREEN}html/css${NC} folder"
    mkdir ./html/css
fi


# check  if need to create html/js folder
if [ -d "./html/js" ]
then
    echo -e "Directory ${GREEN}html/js${NC} exists."
else
    echo -e "Creating ${GREEN}html/js${NC} folder"
    mkdir ./html/js
fi


# check  if need to create html/fonts folder
if [ -d "./html/fonts" ]
then
    echo -e "Directory ${GREEN}html/fonts${NC} exists."
else
    echo -e "Creating ${GREEN}html/fonts${NC} folder"
    mkdir ./html/fonts
fi
        

echo -e "${YELLOW}minifying application files${NC}"
node modules/build.js

echo -e "${YELLOW}File Processing complete${NC}"

PS3="Select OS: "
maps=("Windows" "ARM" "No package")
select m in "${maps[@]}"; do
    case $m in 
        "Windows")
            echo -e "${YELLOW}Creating Windows build${NC}"
            electron-zip-packager . Configurator --overwrite --asar --platform=win32 --arch=x64 --prune=true --out=release-builds
            break
            ;;
        "ARM")
            echo -e "${YELLOW}Creating ARMV71 build${NC}"
            electron-zip-packager . Configurator --overwrite --asar --platform=linux --arch=armv7l --prune=true --out=release-builds
            break
            ;;
        "No package")
            echo -e "${YELLOW}No OS Selected${NC}"
            break
            ;;
        *) echo "invalid option $REPLY";;
    esac
done

echo -e "${YELLOW}Build complete${NC}"