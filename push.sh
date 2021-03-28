#!/bin/sh
if [ "$2" == "+" ]
  then
    v=$(node incriment-version.js);
  else
    v=$(node -p "require('./package.json').version");
fi
sh build.sh
echo "$1, V: $v";
git add .;
git commit -m "$1, V: $v";
git push;
