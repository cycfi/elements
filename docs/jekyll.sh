#!/bin/bash

set -e

if [ ! -d ".bundle" ]; then
	gem update
	gem install bundler
	bundle config set path '.bundle'
	bundle install
fi

bundle exec jekyll serve --watch
