#!/bin/bash

ProjectName=${1:-demo}
BasePath=$( dirname "$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )")
TemplatePath=${BasePath}/template
LibPath=${BasePath}/lib

cp -R ${TemplatePath} ${ProjectName}
pushd ${ProjectName}
ln -s ${LibPath} lib
popd

