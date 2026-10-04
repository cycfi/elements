#!/bin/bash
###############################################################################
#  Copyright (c) 2016-2026 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
# Publishes status grids (from status_grid.py) to status/ on the www branch,
# which the repository's GitHub Pages serve, beside the documentation.
# GH_TOKEN must be able to write the repository's contents.
#
#   publish_status.sh build.svg [more.svg ...]

set -euo pipefail

www=$(mktemp -d)
git clone --quiet --depth 1 --branch www \
   "https://x-access-token:${GH_TOKEN}@github.com/${GITHUB_REPOSITORY}.git" "$www"
mkdir -p "$www/status"
cp "$@" "$www/status/"

cd "$www"
git add status
if git diff --cached --quiet; then
   echo "The status grids are unchanged"
   exit 0
fi
git -c user.name="github-actions[bot]" \
    -c user.email="41898282+github-actions[bot]@users.noreply.github.com" \
    commit --quiet -m "Update the status grids"

# The documentation deploy also pushes to www; take its commit and retry.
for attempt in 1 2 3; do
   git push --quiet origin www && exit 0
   git pull --quiet --rebase origin www
done
exit 1
