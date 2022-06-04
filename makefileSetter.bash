#/bin/bash

#list directories -R
find . -type d -not -path './.*' -exec ls -1R  {} \;