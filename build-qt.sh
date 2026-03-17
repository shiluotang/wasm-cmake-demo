#!/usr/bin/env bash

declare readonly CURDIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd -L)
declare readonly QT_SOURCE_HOME=$HOME/Documents/opensource/qt-everywhere-src-6.10.2

function build_host() {
    local bindir=${CURDIR}/host
    if [[ ! -d ${bindir} ]]; then
        if !  mkdir -p ${bindir}; then
            return 1
        fi
    fi
    if [[ ! -f ${bindir}/CMakeCache.txt ]]; then
        pushd ${bindir} >& /dev/null
        if [[ -x ${QT_SOURCE_HOME}/configure ]]; then
            if ! ${QT_SOURCE_HOME}/configure \
                -release \
                -confirm-license \
                -opensource \
                -nomake examples \
                -nomake tests \
                -prefix ${bindir}/qtbase; then
                return 1
            fi
        fi
        popd >& /dev/null
    fi
    if [[ -f ${bindir}/CMakeCache.txt ]]; then
        pushd ${bindir} >& /dev/null
        if ! cmake --build . --target host_tools; then
            return 1
        fi
        if ! cmake --build . --target Xml; then
            return 1
        fi
        if ! cmake --build . --target Core5Compat; then
            return 1
        fi
        popd >& /dev/null
    fi
}

function build_wasm() {
    local bindir=${CURDIR}/wasm
    if [[ ! -d ${bindir} ]]; then
        if !  mkdir -p ${bindir}; then
            return 1
        fi
    fi
    if [[ ! -f ${bindir}/CMakeCache.txt ]]; then
        pushd ${bindir} >& /dev/null
        if [[ -x ${QT_SOURCE_HOME}/configure ]]; then
            if ! ${QT_SOURCE_HOME}/configure \
                -release \
                -confirm-license \
                -opensource \
                -nomake examples \
                -nomake tests \
                -prefix ${bindir}/qtbase \
                -qt-host-path ${CURDIR}/host/qtbase \
                -platform wasm-emscripten; then
                return 1
            fi
        fi
        popd >& /dev/null
    fi
    if [[ -f ${bindir}/CMakeCache.txt ]]; then
        pushd ${bindir} >& /dev/null
        if ! cmake --build . -t qtbase; then
            return 1
        fi
        if ! cmake --build . -t Xml; then
            return 1
        fi
        if ! cmake --build . -t Core5Compat; then
            return 1
        fi
        popd >& /dev/null
    fi
}

function main() {
    if ! build_host; then
        return 1
    fi
    if ! build_wasm; then
        return 1
    fi
}

main "${@}"
