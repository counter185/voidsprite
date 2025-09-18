$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
Copy-Item "$scriptDir/freesprite/voidsprite_sdk.h" -Destination ".\voidsprite_sdk.h" -Force
$sdkStructsContent = Get-Content "$scriptDir/freesprite/sdk_structs.h"
$sdkStructsContent = $sdkStructsContent -replace '#pragma once', '' -join "`n"
$sdkFileContent = Get-Content ".\voidsprite_sdk.h"
$sdkFileContent = $sdkFileContent -replace '#include "sdk_structs.h"', $sdkStructsContent -join "`n"
Set-Content -Path ".\voidsprite_sdk.h" -Value $sdkFileContent