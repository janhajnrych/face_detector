#ifndef VERSION_H
#define VERSION_H

#define IS_DEBUG 1
namespace BuildInfo {

  const char* getVersion();
  const char* getBuildType();
  inline bool isDebug() {return 1;}

}

#endif
