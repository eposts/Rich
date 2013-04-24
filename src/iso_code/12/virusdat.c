/*
VIRUSDAT.C
这个模块定了_viruscrc变量，默认的搜索变量DEAN_CRC，但是也可以修改这个文件
或者在连接的早期在其他文件定义另外一个_viruscrc
*/


#include "viruscrc.h"


const union filecrc _viruscrc =
  {
  'D', 'E', 'A', 'N', '_', 'C', 'R', 'C'
  };
