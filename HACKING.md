# Hacking

How to get set up hacking on this project.

## You MUST install

1. [git](https://git-scm.com)

## Your life will be easier if you install

1. [hub](https://hub.github.com)

## Forking and cloning this repo

[Fork](https://github.com/pdxjohnny/helios#fork-destination-box) this
repo.

```console
USERNAME=your_github_username
REPO=helios
git clone git@github.com:$USERNAME/$REPO
cd $REPO
git remote add upstream git@github.com:pdxjohnny/$REPO
```

## Grab new changes

```console
# Working on branch XYZ...
git checkout master
git pull upstream master
# Now master is up to date. Star new brnaches off the up to date
# master with
git checkout -b new_branch_name
```

## Making changes with git and GitHib hub

[![asciicast](https://asciinema.org/a/141147.png)](https://asciinema.org/a/141147)

> We've changed the name to helios
> but ECE411 might still work
