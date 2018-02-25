#!/usr/bin/env bash
# Creates assets for sc3-plugins
# Requires: readlink (in coreutils package), git, access to /tmp

set -euo pipefail

get_absolute_path() {
  echo $(dirname $(readlink -f "$0"))
}

create_source_dir(){
  local abs_path="$1"
  mkdir -pv "${abs_path}/source"
  echo "${abs_path}/source"
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
  rm -rfv .gitigonre \
          .git/ \
          .travis.yml \
          website
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
  mv -v "${package_name}-Version-${version}.tar.tgz" "${abs_path}/"
}

upstream="https://github.com/supercollider/sc3-plugins"
package_name="sc3-plugins"
source_dir="/tmp"
os=`uname`
version=`date "+%Y-%m-%d"`
abs_path=$(get_absolute_path $0)

while getopts ":v:s" Option
do
  case $Option in
    v ) version=$OPTARG
        ;;
    s ) package_type="source"
        ;;
  esac
done
shift $(($OPTIND - 1))

checkout_project
checkout_external_libraries
clean_sources
rename_sources
compress_sources
move_sources

exit 0

# vim:set ts=2 sw=2 et:
