#!/usr/bin/env bash

declare readonly CURDIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd -P)

function main() {
    export EM_CONFIG=${EM_CONFIG:-${HOME}/.emscripten}
    declare bindir=build
    pushd ${CURDIR} >& /dev/null
    if [[ ! -d ${bindir} ]]; then
        if ! mkdir ${bindir}; then
            exit
        fi
    fi
    if [[ ! -f ${bindir}/CMakeCache.txt ]]; then
        pushd ${bindir} >& /dev/null
        if ! emcmake cmake ..; then
            exit
        fi
        popd >& /dev/null
    fi
    if [[ -f ${bindir}/Makefile ]]; then
        emmake make -C ${bindir} all test
    fi
    pop >& /dev/null
}

main "${@}"
