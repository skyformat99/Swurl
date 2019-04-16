// Swirl
// Copyright (C) 2019 Steven Mattera
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#pragma once

#include <string>
#include <map>
#include <functional>
#include <curl/curl.h>

#include "WebRequest.hpp"

namespace swirl {
    class SessionManager {
        public:
            static std::string proxyUrl;
            static std::string proxyUsername;
            static std::string proxyPassword;
            static std::string userAgent;
            static std::map<std::string, std::string> requestHeaders;

            static std::function<void(WebRequest *, double)> onProgressChanged;
            static std::function<void(WebRequest *)> onCompleted;
            static std::function<void(WebRequest *, std::string)> onError;

            static void initialize();
            static void dealloc();
            static void makeRequest(WebRequest * request);
        
        private:
            static curl_slist * _generateHeaders(WebRequest * request);
            static std::string _getMethod(WebRequest * request);
            static size_t _writeHeader(const char * in, std::size_t size, std::size_t num, WebRequest * request);
            static size_t _write(const char* in, std::size_t size, std::size_t num, WebRequest * request);
            static size_t _progress(WebRequest * request, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
            static void _parseResponseHeader(WebRequest * request);
    };
}
