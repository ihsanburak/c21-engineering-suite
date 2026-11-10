# Contributing

Original C21-owned code is MIT licensed. Contributions must be original or properly authorized and compatible with the project and component terms; retain upstream notices and identify copied material. Brand artwork is separately reserved.

Open an issue describing the calculator/model, independent reference, units, domain and a reproducible example. For bugs, report hardware/OS/Ndless details, build version, keypad sequence, inputs/units and expected versus actual result. Never include private boot images, TI firmware, credentials or unrelated documents.

Keep mathematical engines separate from UI; use SI internally and explicit unit boundaries. Preserve numeric entry, Formula/List behavior, contextual Handbook state and English/Turkce ASCII. New models require independent numerical references, domain/error checks, complete EN/TR Handbook coverage and native 320×240 layout checks. Do not count inverse outputs as new physical models.

Run `make test HOST_CC=gcc` or the Windows runner and `bash tools/test-sanitized.sh`. Native input tests compile the actual adapter with test-only OS/SDL stubs; they do not emulate the device. Native builds use the genuine Ndless SDK. State host, ARM and physical verification separately.

Do not change supplied brand artwork, relicense dependencies or introduce frameworks without review. Preserve existing tests and releases. Never modify hardware firmware as part of software testing. No public CI/deployment workflow is enabled by this release.
