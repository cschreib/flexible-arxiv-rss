#!/bin/bash

# Read preferences
WORK_PATH=$1
USER_PREFERENCES="${WORK_PATH}/preferences.xml"
USER_CSS_PATH="${WORK_PATH}/arxiv.css"
CACHE_PATH="${WORK_PATH}/cache"
CACHE_HTML_PATH="${CACHE_PATH}/arxiv"
CACHE_RSS_PATH="${CACHE_PATH}/arxiv/rss"
mkdir -p "${CACHE_PATH}"

# Determine the minimum date from which to get postings
# If this is the first poll, just fetch postings that are 2 days old
# Otherwise use the date of the last poll minus one day (to make sure we don't miss anything)
LAST_POLL_PATH="${CACHE_PATH}/oai-lastpoll.txt"
if [ -f "${LAST_POLL_PATH}" ]; then
    DATE_FROM=`cat "${LAST_POLL_PATH}"`
else
    DATE_FROM=`date --date="5 days ago" +%Y-%m-%d`
fi

# Download listing from arxiv using the OAI bulk retrieval protocal
TMP_OUT="${CACHE_PATH}/oai.xml"
echo "Fetching postings from ${DATE_FROM} until today"
wget "http://export.arxiv.org/oai2?verb=ListRecords&from="${DATE_FROM}"&metadataPrefix=arXiv&set=physics:astro-ph" -O "${TMP_OUT}"
if [ $? -ne 0 ]; then
    echo "Could not get postings, aborting"
    exit $?
fi

# Save last poll date into a file for future polling
date --date="1 day ago" +%Y-%m-%d > "${LAST_POLL_PATH}"

# Merge postings into DB
DB_PATH="${CACHE_PATH}/oai-arxiv-db.xml"
${WORK_PATH}/oai-merge-records "${TMP_OUT}" "${DB_PATH}" "${DB_PATH}"
rm -f "${TMP_OUT}"

# Remove old postings and filter them out according to user criteria
UPREF_DATE_CUT=`${WORK_PATH}/oai-get-preference "${USER_PREFERENCES}" keep_history`
DATE_CUT=`date --date="${UPREF_DATE_CUT}" +%Y-%m-%d`
${WORK_PATH}/oai-filter-records "${DB_PATH}" "${USER_PREFERENCES}" ${DATE_CUT}

# Export DB to RSS
rm -rf ${CACHE_HTML_PATH}
mkdir -p "${CACHE_HTML_PATH}"
mkdir -p "${CACHE_RSS_PATH}"
${WORK_PATH}/oai-records-2rss "${DB_PATH}" "${CACHE_RSS_PATH}/feed.rss" "${USER_PREFERENCES}"

# Export DB to HTML
${WORK_PATH}/oai-records-2html "${DB_PATH}" "${CACHE_HTML_PATH}" "${USER_PREFERENCES}"
cp ${USER_CSS_PATH} "${CACHE_HTML_PATH}"/

# Copy files to HTTP server
WWW_DIR=`${WORK_PATH}/oai-get-preference "${USER_PREFERENCES}" http_local_www_path`
rsync -r --delete "${CACHE_HTML_PATH}" "${WWW_DIR}"
