#include "singleton.h"
singleton* singleton::m_pInstance = NULL; 

singleton* singleton::Instance()
{
   if (!m_pInstance)   // Only allow one instance of class to be generated.
      m_pInstance = new singleton;

   return m_pInstance;
}