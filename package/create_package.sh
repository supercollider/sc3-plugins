#!/usr/bin/env bash
# Creates assets for sc3-plugins in the form of
# 'sc3-plugins-x.x.x-Source.tar.bz2' and moves the file to the repository's
# package folder.
# Requires a writable /tmp folder.

set -euo pipefail

get_absolute_path() {
  echo "$(cd "$(dirname "$0")" && pwd -P)"
}

remove_source_dir() {
  echo "Removing potential previous sources."
  rm -rf "${source_dir}/sc3-plugins"*
}

checkout_project() {
  remove_source_dir
  echo "Cloning project..."
  cd "$source_dir"
  git clone $upstream --branch "Version-$version" --single-branch --recursive
}

clean_sources() {
  cd "${source_dir}/${package_name}"
  echo "Removing unneeded files and folders..."
  rm -rfv .gitignore \
          .gitmodules \
          .travis.yml \
          .git/ \
          website \
          package
}

rename_sources() {
  cd "${source_dir}"
  mv -v "${package_name}" "${package_name}-${version}-Source"
}

compress_sources() {
  cd "${source_dir}"
  tar jcvf "${package_name}-${version}-Source.tar.bz2" \
    "${package_name}-${version}-Source"
}

move_sources() {
  cd "${source_dir}"
  mv -v "${package_name}-${version}-Source.tar.bz2" "${output_dir}/"
}

sign_sources() {
  cd "${output_dir}"
  gpg --detach-sign \
      -u "${signer}" \
      -o "${package_name}-${version}-Source.tar.bz2.asc" \
      "${package_name}-${version}-Source.tar.bz2"
}

cleanup_source_dir() {
  cd "${source_dir}"
  rm -rf "${package_name}-${version}"
}

print_help() {
  echo "Usage: $0 -v <version tag> -s <signature email>"
  exit 1
}

upstream="https://github.com/supercollider/sc3-plugins"
package_name="sc3-plugins"
source_dir="/tmp"
version=`date "+%Y-%m-%d"`
signer=""
signature=0
output_dir=$(get_absolute_path $0)

if [ ${#@} -gt 0 ]; then
  while getopts 'hv:s:' flag; do
    case "${flag}" in
      h) print_help
          ;;
      s) signer=$OPTARG
         signature=1
          ;;
      v) version=$OPTARG
          ;;
      *)
        echo "Error! Try '${0} -h'."
        exit 1
        ;;
    esac
  done
else
  print_help
fi

checkout_project
clean_sources
rename_sources
compress_sources
move_sources
if [ $signature -eq 1 ]; then
  sign_sources
fi
cleanup_source_dir

exit 0

# vim:set ts=2 sw=2 et:
