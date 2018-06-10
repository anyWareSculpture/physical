#ifndef CONTROLLERS_H_
#define CONTROLLERS_H_

#define str(s) #s
#define STRIPREGEXP(id) " " str(id)
#if STRIPID == 0
  #include "ControllerA.h"
#elif STRIPID == 1
  #include "ControllerB.h"
#elif STRIPID == 2
  #include "ControllerC.h"
#endif

#endif
