#!/bin/bash
set -e

git config alias.todo '!grep --color=auto -rn -A 10 TODO --exclude=*install.sh --exclude=*config --binary-files=without-match'
cp scripts/hooks/* .git/hooks/
