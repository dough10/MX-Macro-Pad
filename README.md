# MX Macro-Pad

open source "Pikatea Macropad Clone"
A Macro Pad made with Cherry MX switches pre switch 3mm LEDs 1 encoder and a SD card reader for data storage based on a Arduino Micro. Includes a electron html5 UI for changing key binds.

This is my first Arduino project and would love some help / guidance on what has to be some pretty inefficient code.

all pin settings for Arduino project are in the MacroPad.ino file.

This macro pad can do up to 3 button macro's. I would like to include the option to use media keys. It appears I have over estimated the capabilities of the Arduino micro. When HID lib in enabled I can not upload the script using the Arduino editor. 102% of flash. =(  for the lib to work I think I would also need to change the key values to Strings in place of using the decimal values.

I have included the schematic I used to build my project.
[https://github.com/dough10/MX-Macro-Pad/blob/master/Project%20Schematic/Schematic_mx%20macro_2020-09-13_10-54-19.pdf](https://github.com/dough10/MX-Macro-Pad/blob/master/Project%20Schematic/Schematic_mx%20macro_2020-09-13_10-54-19.pdf)

Required software for UI development environment

- Nodejs
- NPM
- Text Editor

Required software for Arduino development environment

- Arduino compatable IDE

Steps to install development environment

- cd into directory "cd MX Macro-Pad"
- run "npm install"
- rebuild for electron compatibility './node_modules/.bin/electron-rebuild.cmd'
- run "npm test" if all was successful the app will load in a electron window
- have fun

commands

- npm test - runs the electron app using files from "src" folder
- npm start - runs the electron app using minified files from "html" folder

to build a windows desktop app

- from project root dir run "sh build.sh"  -- if on windows you will need something like git bash
