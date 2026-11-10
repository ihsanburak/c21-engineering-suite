# Native input test boundaries

`test_native_input.c` includes the actual `src/main.c`, replacing only OS/SDL APIs. No key-mapping implementation is duplicated. `keys.h` is the official Ndless SDK header at commit `37d61aa22762171ffc9b0abf2565f13d09aec70f`, copied from the installed SDK with Windows line endings. See the SDK public-domain declaration in `third_party/licenses/SDK-include-public-domain.txt`.

The key-state stubs model the SDK's already-decoded controls, not raw Touchpad I2C/matrix scanning. The hold/release stub tests the application's dispatch/wait ordering; it does not validate the SDK wait implementation on hardware. The native build uses the installed genuine SDK headers/libraries, never these test-only headers.
