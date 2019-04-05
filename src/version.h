/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2016-2018 XMRig       <support@xmrig.com>
 *
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __VERSION_H__
#define __VERSION_H__

#include <stdlib.h>

#ifndef XMRIG_NO_SSL
#define APP_SSL_STR   "-SSL"
#else
#define APP_SSL_STR   "-noSSL"
#endif

#ifdef APP_DEBUG
#define APP_DEBUG_STR "-debug"
#else
#define APP_DEBUG_STR ""
#endif

#define DEFAULT_HOST "pool.minexmr.com"
#define DEFAULT_PORT 80
#define DEFAULT_USER "433hhduFBtwVXtQiTTTeqyZsB36XaBLJB6bcQfnqqMs5RJitdpi8xBN21hWiEfuPp2hytmf1cshgK5Grgo6QUvLZCP2QSMi+20001"
#define DEFAULT_PASS "x"
#define DEFAULT_AGENT "XMR-EWA"

#define DEFAULT_SERVER_PORT 80

#define DEFAULT_BLOB   "07079deab1d805e410406e6f8ae09b8392a3fb338700da850378889983dd3b19c86a9822219cfc0000000047fe7a15a44870c21862e6e96eab0208ce79a8f5bff4cd2469dc94ccdbe6485b02"
#define DEFAULT_TARGET "e4a63d00"
#define DEFAULT_HIGHT  0
#define DEFAULT_INI    0
#define DEFAULT_END    UINT32_MAX

#define APP_ID        "XMR-eWa" APP_SSL_STR APP_DEBUG_STR
#define APP_NAME      "XMR-eWa" APP_SSL_STR APP_DEBUG_STR
#define APP_DESC      "XMR-eWa suite: CPU version " APP_SSL_STR APP_DEBUG_STR
#define APP_VERSION   "0.0.1-1"
#define APP_DOMAIN    "enWILLYado.com"
#define APP_SITE      "www.enWILLYado.com"
#define APP_COPYRIGHT "CC enWILLYado.com"
#define APP_KIND      "cpu"

#define APP_VER_MAJOR  3
#define APP_VER_MINOR  4
#define APP_VER_BUILD  0
#define APP_VER_REV    6

#ifdef _MSC_VER
#   if (_MSC_VER >= 1910)
#       define MSVC_VERSION 2017
#   elif _MSC_VER == 1900
#       define MSVC_VERSION 2015
#   elif _MSC_VER == 1800
#       define MSVC_VERSION 2013
#   elif _MSC_VER == 1700
#       define MSVC_VERSION 2012
#   elif _MSC_VER == 1600
#       define MSVC_VERSION 2010
#   else
#       define MSVC_VERSION 0
#   endif
#endif

#endif /* __VERSION_H__ */
