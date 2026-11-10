#!/usr/bin/env bash
set -euo pipefail
export ASAN_OPTIONS="${ASAN_OPTIONS:-detect_leaks=1}"
export UBSAN_OPTIONS="${UBSAN_OPTIONS:-halt_on_error=1}"
cd "$(dirname "$0")/.."
mkdir -p build
gcc -std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_host.c \
  -lm -o build/test-host-sanitized
./build/test-host-sanitized

gcc -std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_interactive.c \
  -lm -o build/test-interactive-sanitized
./build/test-interactive-sanitized

gcc -std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_formula_editor.c -lm -o build/test-formula-editor-sanitized
./build/test-formula-editor-sanitized

gcc -std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_mono_ui.c -lm -o build/test-mono-ui-sanitized
./build/test-mono-ui-sanitized

gcc -std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_localization.c -lm -o build/test-localization-sanitized
./build/test-localization-sanitized

gcc -std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_ascii.c -lm -o build/test-ascii-sanitized
./build/test-ascii-sanitized

gcc -std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_expansion.c -lm -o build/test-expansion-sanitized
./build/test-expansion-sanitized

gcc -std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_rf.c -lm -o build/test-rf-sanitized
./build/test-rf-sanitized

gcc -std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_lab.c -lm -o build/test-lab-sanitized
./build/test-lab-sanitized

gcc -std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_lora.c -lm -o build/test-lora-sanitized
./build/test-lora-sanitized

gcc -std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer -Itests/native_stubs -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_native_input.c -lm -o build/test-native-input-sanitized
./build/test-native-input-sanitized

gcc -std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_language_menu.c -lm -o build/test-language-menu-sanitized
./build/test-language-menu-sanitized
