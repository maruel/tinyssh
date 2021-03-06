#!/bin/sh -e

build="`pwd`/build"
source="`pwd`"
bin="${build}/bin"
installcmd="${bin}/_tinysshd-install"

cat "${source}/tinyssh/TARGETS" |\
while read x
do
  [ -x "${bin}/${x}" ] || \
    ( 
      echo "=== `date` === $x not compiled, compile first!"
      exit 111; 
    ) || exit 111
done || exit 111

[ -x "${installcmd}" ] || \
    ( 
      echo "=== `date` === ${installcmd} not compiled, compile first!"
      exit 111; 
    ) || exit 111

#bin
confbin="`head -1 conf-bin`"
bindir=`echo "$1/${confbin}" | sed 's,//,/,g'`
echo "=== `date` === installing bin directory ${bindir}"
mkdir -p "${bindir}" || exit 111
cat "${source}/tinyssh/TARGETS" |\
while read x
do
  #cp "${bin}/${x}" "$1/${confbin}" || exit 111
  #chmod 755 "$1/${confbin}/${x}" || exit 111
  #chown 0:0 "$1/${confbin}/${x}" || exit 111
  "${installcmd}" "${bin}/${x}" "${bindir}" "${x}.tmp" "${x}" || exit 111
  echo "=== `date` ===   installing ${bin}/${x} -> ${bindir}/${x}"
done || exit 111
echo "=== `date` === finishing"

exit 0
