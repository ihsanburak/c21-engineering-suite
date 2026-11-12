$ErrorActionPreference='Stop'
Push-Location (Split-Path -Parent $PSScriptRoot)
try {
 $previewExe=Join-Path (Get-Location) ("build/host-preview-"+[DateTime]::UtcNow.ToString('yyyyMMddHHmmssfff')+".exe")
 & "$PSScriptRoot/host-compiler/tcc/tcc.exe" -std=c99 -Wall -Isrc -Iassets src/splash_anim.c src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tools/host-preview.c -o $previewExe
 if($LASTEXITCODE -ne 0){throw 'Host preview compilation failed'}
 & py -3.13 tools/preview-ui.py --engine $previewExe @args
 if($LASTEXITCODE -ne 0){throw 'Host preview failed'}
} finally { Pop-Location }
