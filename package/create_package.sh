#!/usr/bin/env bash
# Creates assets for sc3-plugins in the form of
# 'sc3-plugins-Version-x.x.x.tar.gz' and moves the file to the repository's
# package folder.
# Requires a writable /tmp folder.

set -euo pipefail

get_absolute_path() {
  echo "$(cd "$(dirname "$0")" && pwd -P)"
}

remove_source_dir(){
  echo "Removing potential previous sources."
  rm -rf "${source_dir}/sc3-plugins"*
}

checkout_project() {
  remove_source_dir
  echo "Cloning project..."
  cd "$source_dir"
  git clone $upstream
  echo "Checking out version: Version-$version"
  cd "$package_name"
  git checkout "Version-$version"
}

checkout_external_libraries() {
  echo "Checking out external libraries..."
  cd "${source_dir}/${package_name}"
  git submodule update --init --recursive
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
  mv -v "${package_name}" "${package_name}-Version-${version}"
}

compress_sources() {
  cd "${source_dir}"
  tar cvfz "${package_name}-Version-${version}.tar.tgz" \
    "${package_name}-Version-${version}"
}

move_sources() {
  cd "${source_dir}"
  mv -v "${package_name}-Version-${version}.tar.tgz" "${output_dir}/"
}

cleanup_source_dir() {
  cd "${source_dir}"
  rm -rf "${package_name}-Version-${version}"
}

print_help() {
  echo "Usage: $0 -v <version tag>"
  exit 1
}

upstream="https://github.com/supercollider/sc3-plugins"
package_name="sc3-plugins"
source_dir="/tmp"
version=`date "+%Y-%m-%d"`
output_dir=$(get_absolute_path $0)

if [ ${#@} -gt 0 ]; then
  while getopts 'hv:' flag; do
    case "${flag}" in
      h) print_help
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
checkout_external_libraries
clean_sources
rename_sources
compress_sources
move_sources
cleanup_source_dir

exit 0

# vim:set ts=2 sw=2 et:
