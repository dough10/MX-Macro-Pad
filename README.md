# Mechanical-Macro-Pad


Required software for UI development environment
  - Nodejs
  - NPM
  - Text Editor

Required software for Arduino development environment
 - Arduino compatable IDE


Steps to install development environment
  - cd into directory "cd Mechanical-Macro-Pad"
  - run "npm install"
  - rebuild for electron compatibility './node_modules/.bin/electron-rebuild.cmd'
  - run "npm test" if all was successful the app will load in a electron window
  - have fun hacking away at it

commands
  - npm test - runs the electron app using files from "src" folder
  - npm start - runs the electron app using minified files from "html" folder


to build a windows desktop app
  - from project root dir run "sh build.sh"  -- if on windows you will need something like git bash
