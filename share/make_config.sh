#!/bin/bash

OUTPUT_DIR=$1
CONFIG_DIR=$2
USER_HTTP_PORT=$3
WWW_PATH=$4
NGINX_CACHE=$5

simple_sed() {
    local file=$1
    local key=`echo $2 | sed -e 's/\[/\\\[/g' | sed -e 's/\]/\\\]/g'`
    local rep=$3

    cat "${file}" | sed 's|'${key}'|'${rep}'|g' > tmp.file
    mv tmp.file ${file}
}

cp ../share/preferences.xml "${OUTPUT_DIR}"/preferences.xml
cp ../share/nginx.conf "${OUTPUT_DIR}"/nginx.conf
cp ../share/poll_arxiv_oai.sh "${OUTPUT_DIR}"/poll_arxiv_oai.sh

simple_sed "${OUTPUT_DIR}"/preferences.xml "[[WWW_PATH]]" "${WWW_PATH}"
simple_sed "${OUTPUT_DIR}"/preferences.xml "[[USER_HTTP_PORT]]" "${USER_HTTP_PORT}"
simple_sed "${OUTPUT_DIR}"/nginx.conf "[[WWW_PATH]]" "${WWW_PATH}"
simple_sed "${OUTPUT_DIR}"/nginx.conf "[[USER_HTTP_PORT]]" "${USER_HTTP_PORT}"
simple_sed "${OUTPUT_DIR}"/nginx.conf "[[NGINX_CACHE]]" "${NGINX_CACHE}"
simple_sed "${OUTPUT_DIR}"/poll_arxiv_oai.sh "[[CONFIG_DIR]]" "${CONFIG_DIR}"
