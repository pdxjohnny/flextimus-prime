#!/bin/bash

grep -qq 'set auto-load safe-path /' "$HOME/.gdbinit"
_ret=$?
if [[ "$_ret" != "0" ]]; then
  echo "set auto-load safe-path /" >> "$HOME/.gdbinit"
fi
git config alias.todo '!grep --color=auto -rn -A 10 TODO --exclude=*install.sh --exclude=*config --binary-files=without-match'
cp scripts/hooks/* .git/hooks/
