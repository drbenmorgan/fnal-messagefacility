#!/bin/bash

status=0

prefix=$CETPKG_SOURCE/test/MessageService
refsubdir=$prefix/output-references/ELdestinationTester

for testdir in `find -type d -wholename "*test/MessageService/*.d"`
do

    # If testdir is not empty, redact timestamps, which will not be
    # the same wrt reference
    if test "$(ls -A "$testdir")"; then
      for file in $testdir/*
      do
          sed -r -f $prefix/filter-timestamps.sed $file > tmp
          mv tmp $file
      done
    fi

    # Compare outputs
    dirSuffix=${testdir#*messagefacility_}
    dirId=${dirSuffix%_t.d}
    refdir=$refsubdir/$dirId
    diff -rq $testdir $refdir

done

exit $status
