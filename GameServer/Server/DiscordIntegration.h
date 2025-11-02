#ifndef DISCORD
#define DISCORD
#pragma once
#define CURL_STATICLIB
#include "../Curl/curl.h"
#pragma comment(lib, "curl/libcurl.lib")
#pragma comment(lib, "curl/zlib.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "crypt32.lib")
#include "Utils.h"

namespace Messages
{
    std::string HostStarted = "{\"content\":null,\"embeds\":[{\"title\":\"Started %host%\",\"description\":\"**This is just a test!**\\n```open 0```\\n<@&1409988482184642710> <@&1411067122116988949>\",\"color\":10027263,\"thumbnail\":{\"url\":\"https://images-ext-1.discordapp.net/external/kzqhN1GkAJDG1UTPd41F9eQMeDJBbCTNMvrY7C1ttU0/%3Fsize%3D4096/https/cdn.discordapp.com/icons/1409675217051324462/a02f52806f9df4f4e32a905eac644596.png?format=webp&quality=lossless&width=351&height=351\"}}],\"attachments\":[]}";
    std::string HostRestarting = "{\"content\":null,\"embeds\":[{\"title\":\"Restarting %host%\",\"description\":\"**Server is restarting!**\\nThanks for playing!\",\"color\":10027263,\"thumbnail\":{\"url\":\"https://images-ext-1.discordapp.net/external/kzqhN1GkAJDG1UTPd41F9eQMeDJBbCTNMvrY7C1ttU0/%3Fsize%3D4096/https/cdn.discordapp.com/icons/1409675217051324462/a02f52806f9df4f4e32a905eac644596.png?format=webp&quality=lossless&width=351&height=351\"}}],\"attachments\":[]}";
}
bool Replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}


void SendWebhook(std::string msg)
{
    static bool Once = false;
    static CURL* curl = nullptr;
    if (!Once)
    {
        Once = true;
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/webhooks/1428870205853143131/C0WKrRc3qllsWyH0nurN5RzTI08Ea7mbtcmf-fDVwk9h1afrJ-rd8JtmcnLYZB7fTy4g");
            curl_slist* headers = curl_slist_append(NULL, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }
    }

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, msg.c_str());
    curl_easy_perform(curl);
}
#endif
