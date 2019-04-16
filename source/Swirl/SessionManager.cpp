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

#include <sstream>
#include <switch.h>

#include "SessionManager.hpp"
#include "WebResponse.hpp"

using namespace std;

namespace swirl {
    std::string SessionManager::proxyUrl = "";
    std::string SessionManager::proxyUsername = "";
    std::string SessionManager::proxyPassword = "";
    std::string SessionManager::userAgent = "";
    std::map<std::string, std::string> SessionManager::requestHeaders;

    std::function<void(WebRequest *, double)> SessionManager::onProgressChanged;
    std::function<void(WebRequest *)> SessionManager::onCompleted;
    std::function<void(WebRequest *, std::string)> SessionManager::onError;

    void SessionManager::initialize() {
        socketInitializeDefault();
        curl_global_init(CURL_GLOBAL_ALL);
    }

    void SessionManager::dealloc() {
        curl_global_cleanup();
        socketExit();
    }

    void SessionManager::makeRequest(WebRequest * request) {
        CURL * curl = curl_easy_init();
        if (!curl) {
            onError(request, "Unable to initialize CURL.");
            return;
        }

        struct curl_slist * headers = _generateHeaders(request);
        if (headers != NULL) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }

        if (userAgent.size() > 0) {
            curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
        }

        if (proxyUrl.size() > 0) {
            curl_easy_setopt(curl, CURLOPT_PROXY, proxyUrl.c_str());

            if (proxyUsername.size() > 0) {
                curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, proxyUsername.c_str());

                if (proxyPassword.size() > 0)
                    curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, proxyPassword.c_str());
            }
        }

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, _getMethod(request).c_str());
        curl_easy_setopt(curl, CURLOPT_URL, request->url.c_str());
        // TODO: For this to work you have to pass CURL a CA bundle.
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        if (!request->sslVerifyHost) {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, _writeHeader);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *) request);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) request);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, _progress);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, (void *) request);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            onError(request, string(curl_easy_strerror(res)));

            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
            return;
        }

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &request->response.statusCode);

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        _parseResponseHeader(request);
        onCompleted(request);
    }


    curl_slist * SessionManager::_generateHeaders(WebRequest * request) {
        struct curl_slist * result = NULL;

        // Add the global headers.
        map<string, string>::iterator glIt;
        for (glIt = requestHeaders.begin(); glIt != requestHeaders.end(); glIt++) {
            // Skip global headers that have been overwritten in the request.
            auto findReqIt = request->headers.find(glIt->first);
            if (findReqIt == request->headers.end()) {
                result = curl_slist_append(result, (glIt->first + ": " + glIt->second).c_str());
            }
        }

        // Add the request headers.
        map<string, string>::iterator reqIt;
        for (reqIt = request->headers.begin(); reqIt != request->headers.end(); reqIt++) {
            result = curl_slist_append(result, (reqIt->first + ": " + reqIt->second).c_str());
        }

        return result;
    }

    string SessionManager::_getMethod(WebRequest * request) {
        switch (request->method) {
            case GET:
                return "GET";

            case POST:
                return "POST";

            case PUT:
                return "PUT";

            case DELETE:
                return "DELETE";
        }

        return NULL;
    }

    size_t SessionManager::_writeHeader(const char * in, size_t size, size_t num, WebRequest * request) {
        const size_t totalBytes(size * num);
        request->response.rawResponseHeader.append(in, totalBytes);
        return totalBytes;
    }

    size_t SessionManager::_write(const char * in, size_t size, size_t num, WebRequest * request) {
        const size_t totalBytes(size * num);
        request->response.rawResponseBody.append(in, totalBytes);
        return totalBytes;
    }

    size_t SessionManager::_progress(WebRequest * request, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
        auto progress = (double) dlnow / (double) dltotal;
        if (onProgressChanged) {
            onProgressChanged(request, progress);
        }

        return 0;
    }

    void SessionManager::_parseResponseHeader(WebRequest * request) {
        if (request->response.rawResponseHeader.size() != 0) {
            istringstream ss(request->response.rawResponseHeader);
            string header;
            while(getline(ss, header)) {
                if (header.size() == 0)
                    continue;
                
                auto colonPos = header.find(":");
                if (colonPos != string::npos) {
                    request->response.headers.insert(
                        pair<string, string>(
                            header.substr(0, colonPos),
                            header.substr(colonPos + 2, header.size() - (colonPos + 2))
                        )
                    );
                }
            }
        }
    }
}
