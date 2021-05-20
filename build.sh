#!/bin/bash

CODE='\033['
BLACK=$CODE'0;30m'
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

electronBuild () {
  electron-zip-packager . Configurator --overwrite --asar --platform=$1 --arch=$2 --prune=true --out=release-builds
}

checkFolder () {
  if [ -d "./$1" ]
  then
    echo -e "Directory ${GREEN}$1${NC} exists."
  else
    echo -e "Creating ${GREEN}$1${NC} folder"
    mkdir ./$1
  fi 
}

# sets to use openauto pro keymap
useOAPMap () {
  echo -e "Using ${BLUE}Openauto pro${NC} keymap"
  cp keymaps/oap.json src/keys.json
}

# sets keymap as default
useDefaultMap () {
  echo -e "Using ${BLUE}Default${NC} keymap"
  cp keymaps/default.json src/keys.json
}

userQuit () {
  echo -e "Exiting"
  exit
}

buildWindows () {
  echo -e "${YELLOW}Creating Windows build${NC}"
  electronBuild "win32" "x64"
}

buildARM () {
  echo -e "${YELLOW}Creating ARMV71 build${NC}"
  electronBuild "linux" "armv7l"
} 

noBuild () {
  echo -e "${YELLOW}No OS Selected${NC}"
}

askUser () {
  PS3="$1: "
  maps=($2 $3 $4)
  select m in "${maps[@]}"; do
    case $m in
    $2)
      $5
      break
      ;;
    $3)
      $6
      break
      ;;
    $4)
      $7
      break
      ;;
    *) 
      echo "invalid option $REPLY";;
    esac
  done 
}

askUser "Choose keymap" "Openauto" "Default" "Exit" useOAPMap useDefaultMap userQuit

checkFolder "html"

checkFolder "html/css"

checkFolder "html/js"

checkFolder "html/fonts"

echo -e "${YELLOW}minifying application files${NC}"
node modules/build.js

echo -e "${YELLOW}File Processing complete${NC}"

askUser "Select OS" "Windows" "ARM" "Exit" buildWindows buildARM noBuild

echo -e "${YELLOW}Build complete${NC}"
