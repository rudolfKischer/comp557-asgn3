#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/rudolfkischer/MCGILL/COMP557/Projects/COMP557-asgn3/comp557-asgn3/build
  make -f /Users/rudolfkischer/MCGILL/COMP557/Projects/COMP557-asgn3/comp557-asgn3/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/rudolfkischer/MCGILL/COMP557/Projects/COMP557-asgn3/comp557-asgn3/build
  make -f /Users/rudolfkischer/MCGILL/COMP557/Projects/COMP557-asgn3/comp557-asgn3/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/rudolfkischer/MCGILL/COMP557/Projects/COMP557-asgn3/comp557-asgn3/build
  make -f /Users/rudolfkischer/MCGILL/COMP557/Projects/COMP557-asgn3/comp557-asgn3/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/rudolfkischer/MCGILL/COMP557/Projects/COMP557-asgn3/comp557-asgn3/build
  make -f /Users/rudolfkischer/MCGILL/COMP557/Projects/COMP557-asgn3/comp557-asgn3/build/CMakeScripts/ReRunCMake.make
fi

