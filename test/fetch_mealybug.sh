#!/bin/sh
# Fetch the Mealybug Tearoom Tests: the prebuilt ROMs and the reference
# screenshots. Everything lands under test/mealybug/, which is gitignored.
set -e

dir=$(dirname "$0")/mealybug
upstream=$dir/upstream
repo=https://github.com/mattcurrie/mealybug-tearoom-tests.git

if [ ! -d "$upstream" ]; then
    echo "cloning $repo"
    git clone --depth 1 "$repo" "$upstream"
fi

mkdir -p "$dir/roms"
unzip -oq "$upstream/mealybug-tearoom-tests.zip" -d "$dir/roms"
echo "$(ls "$dir/roms" | wc -l) roms in $dir/roms"
