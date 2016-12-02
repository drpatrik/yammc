#!/bin/bash
ctags -e -R --extra=+fq --exclude=db --exclude=gtest --exclude=.git --exclude=public -f TAGS
