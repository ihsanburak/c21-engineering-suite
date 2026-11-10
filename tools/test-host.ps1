param([string]$Compiler = "$PSScriptRoot\host-compiler\tcc\tcc.exe")
$ErrorActionPreference='Stop'
Push-Location (Split-Path -Parent $PSScriptRoot)
try {
 New-Item -ItemType Directory -Force build | Out-Null
 & $Compiler -std=c99 -Wall -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_host.c -o build/test-host.exe
 if($LASTEXITCODE -ne 0){throw 'Host compilation failed'}
 & .\build\test-host.exe
 if($LASTEXITCODE -ne 0){throw 'Host tests failed'}
 & $Compiler -std=c99 -Wall -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_interactive.c -o build/test-interactive.exe
 if($LASTEXITCODE -ne 0){throw 'Interactive test compilation failed'}
 & .\build\test-interactive.exe
 if($LASTEXITCODE -ne 0){throw 'Interactive tests failed'}
 & $Compiler -std=c99 -Wall -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_formula_editor.c -o build/test-formula-editor.exe
 if($LASTEXITCODE -ne 0){throw 'Formula editor test compilation failed'}
 & .\build\test-formula-editor.exe
 if($LASTEXITCODE -ne 0){throw 'Formula editor tests failed'}
 & $Compiler -std=c99 -Wall -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_mono_ui.c -o build/test-mono-ui.exe
 if($LASTEXITCODE -ne 0){throw 'Mono UI compilation failed'}
 & .\build\test-mono-ui.exe
 if($LASTEXITCODE -ne 0){throw 'Mono UI tests failed'}
 & $Compiler -std=c99 -Wall -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_localization.c -o build/test-localization.exe
 if($LASTEXITCODE -ne 0){throw 'Localization compilation failed'}
 & .\build\test-localization.exe
 if($LASTEXITCODE -ne 0){throw 'Localization tests failed'}
 & $Compiler -std=c99 -Wall -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_ascii.c -o build/test-ascii.exe
 if($LASTEXITCODE -ne 0){throw 'ASCII test compilation failed'}
 & .\build\test-ascii.exe
 if($LASTEXITCODE -ne 0){throw 'ASCII tests failed'}
 & $Compiler -std=c99 -Wall -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_expansion.c -o build/test-expansion.exe
 if($LASTEXITCODE -ne 0){throw 'Expansion compilation failed'}
 & .\build\test-expansion.exe
 if($LASTEXITCODE -ne 0){throw 'Expansion tests failed'}
 & $Compiler -std=c99 -Wall -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_rf.c -o build/test-rf.exe
 if($LASTEXITCODE -ne 0){throw 'RF test compilation failed'}
 & .\build\test-rf.exe
 if($LASTEXITCODE -ne 0){throw 'RF tests failed'}
 & $Compiler -std=c99 -Wall -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_lab.c -o build/test-lab.exe
 if($LASTEXITCODE -ne 0){throw 'PCB/lab compilation failed'}
 & .\build\test-lab.exe
 if($LASTEXITCODE -ne 0){throw 'PCB/lab tests failed'}
 & $Compiler -std=c99 -Wall -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_lora.c -o build/test-lora.exe
 if($LASTEXITCODE -ne 0){throw 'LoRa/RC compilation failed'}
 & .\build\test-lora.exe
 if($LASTEXITCODE -ne 0){throw 'LoRa/RC tests failed'}
 & $Compiler -std=c99 -Wall -Itests/native_stubs -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_native_input.c -o build/test-native-input.exe
 if($LASTEXITCODE -ne 0){throw 'Native input compilation failed'}
 & .\build\test-native-input.exe
 if($LASTEXITCODE -ne 0){throw 'Native input tests failed'}
 & $Compiler -std=c99 -Wall -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_language_menu.c -o build/test-language-menu.exe
 if($LASTEXITCODE -ne 0){throw 'Language menu compilation failed'}
 & .\build\test-language-menu.exe
 if($LASTEXITCODE -ne 0){throw 'Language menu tests failed'}
} finally { Pop-Location }
