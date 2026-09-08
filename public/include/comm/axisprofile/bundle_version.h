#pragma once

// ---------------------------------------------------------------------------
//
//! @file       bundle_version.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2019
//
// ---------------------------------------------------------------------------
//
//! This file is part of the axis profile Library and defines the current
//! bundle version. It's modified during the build process.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    // ---------------------------------------------------------------------------
    // Declaration of symbolic constants
    // ---------------------------------------------------------------------------

    struct VersionT
    {
      const unsigned int VERSION_MAJOR;     //!< major version
      const unsigned int VERSION_MINOR;     //!< minor version
      const unsigned int VERSION_PATCH;     //!< patch version
      const unsigned int VERSION_BUILD;     //!< build version
      const char *BRANCH;                   //!< current branch
      const char *VERSION_STRING;           //!< complete version as string
      const char *GIT_COMMIT;               //!< git commit
    };

    const static VersionT s_version{
      // DON'T REMOVE THE NEXT LINE !!!
      // VERSION_DEFINES_START
      0,                                                //!< major version
      0,                                                //!< major version
      1,                                                //!< major version
      999,                                              //!< major version
      "local_build",                                    //!< current branch
      "0.0.1-local_build.999",                          //!< complete version as string
      "d1403afe8fdf63fbc83e7d628ad5ea42fce7ced0"        //!< git commit
      // VERSION_DEFINES_END
      // DON'T REMOVE THE PREVIOUS LINE !!!
    };
  }
}
