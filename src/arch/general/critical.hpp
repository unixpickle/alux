#ifndef __GENERAL_CRITICAL_HPP__
#define __GENERAL_CRITICAL_HPP__

namespace OS {

bool IgnoreCriticality();
bool GetCritical();
void SetCritical(bool flag);

}

#endif
