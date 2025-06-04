#include "pch.h"
#include "voidsprite_sdk.h"

#include <stdio.h>

void pluginInit(voidspriteSDK* sdk)
{
    printf("Hello from pluginInit\n");
}

const char* getPluginName()
{
    return "voidsprite sample plugin";
}

const char* getPluginVersion()
{
    return "v1.0";
}

const char* getPluginDescription()
{
    return "cool plugin that does things";
}

const char* getPluginAuthors()
{
    return "me";
}
