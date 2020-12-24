#!/bin/sh

if [ "$1" = "${CMAKE_CXX_COMPILER}" ]; then
    shift
fi

export CCACHE_CPP2=true
exec "${CXX_LAUNCHER}" "${CMAKE_CXX_COMPILER}" "$@"
