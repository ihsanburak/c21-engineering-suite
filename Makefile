# Follows the official Ndless helloworld-sdl ELF -> Zehn -> make-prg flow.
CC = nspire-gcc
LD = nspire-ld
CFLAGS = -std=c99 -Os -Wall -Wextra -marm -Isrc -Iassets -MMD -MP
OBJECTS = build/main.o build/engine.o build/rf_engine.o build/lab_engine.o build/lora_engine.o build/display.o build/ui.o build/handbook.o build/math_layout.o build/numeric_input.o build/locale.o build/settings.o
TARGET = build/c21-engineering-suite
.PHONY: all check-tools check-assets test clean
all: $(TARGET).tns
check-tools:
	@command -v nspire-gcc >/dev/null || { echo 'Missing Ndless SDK: see docs/BUILD.md'; exit 1; }
	@command -v genzehn >/dev/null
	@command -v make-prg >/dev/null
check-assets:
	@test -f assets/c21_logo.h || { echo 'Missing original Eagle assets. See assets/README.md'; exit 1; }
build/%.o: src/%.c | check-tools check-assets
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET).elf: $(OBJECTS)
	$(LD) $^ -o $@ -lm -Wl,-Map,build/c21-engineering-suite.map
$(TARGET).tns: $(TARGET).elf
	genzehn --input $< --output $@.zehn --name "C21 Engineering Suite"
	make-prg $@.zehn $@
test:
	mkdir -p build
	$(HOST_CC) -std=c99 -Wall -Wextra -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_host.c -o build/test-host -lm
	./build/test-host
	$(HOST_CC) -std=c99 -Wall -Wextra -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_interactive.c -o build/test-interactive -lm
	./build/test-interactive
	$(HOST_CC) -std=c99 -Wall -Wextra -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_formula_editor.c -o build/test-formula-editor -lm
	./build/test-formula-editor
	$(HOST_CC) -std=c99 -Wall -Wextra -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_mono_ui.c -o build/test-mono-ui -lm
	./build/test-mono-ui
	$(HOST_CC) -std=c99 -Wall -Wextra -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_localization.c -o build/test-localization -lm
	./build/test-localization
	$(HOST_CC) -std=c99 -Wall -Wextra -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_ascii.c -o build/test-ascii -lm
	./build/test-ascii
	$(HOST_CC) -std=c99 -Wall -Wextra -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_expansion.c -o build/test-expansion -lm
	./build/test-expansion
	$(HOST_CC) -std=c99 -Wall -Wextra -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_rf.c -o build/test-rf -lm
	./build/test-rf
	$(HOST_CC) -std=c99 -Wall -Wextra -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_lab.c -o build/test-lab -lm
	./build/test-lab
	$(HOST_CC) -std=c99 -Wall -Wextra -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_lora.c -o build/test-lora -lm
	./build/test-lora
	$(HOST_CC) -std=c99 -Wall -Wextra -Itests/native_stubs -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_native_input.c -o build/test-native-input -lm
	./build/test-native-input
	$(HOST_CC) -std=c99 -Wall -Wextra -Isrc -Iassets src/engine.c src/rf_engine.c src/lab_engine.c src/lora_engine.c src/display.c src/ui.c src/handbook.c src/math_layout.c src/numeric_input.c src/locale.c src/settings.c tests/test_language_menu.c -o build/test-language-menu -lm
	./build/test-language-menu
HOST_CC ?= cc
clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) $(TARGET).elf $(TARGET).tns $(TARGET).tns.zehn
-include $(OBJECTS:.o=.d)
