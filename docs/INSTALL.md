# Installation — 1.0 Classic Edition

Application build: **1.0.2**. Target: original monochrome TI-Nspire Touchpad, non-CAS, OS 3.9.0.463, with already-working compatible Ndless. No other configuration is verified.

1. Obtain the versioned GitHub release package and verify its checksums before transferring anything.
2. Keep the previous application and your existing documents. Transfer only `C21_Engineering_Suite_1.0_Classic_Edition.tns` using your existing document-transfer setup into the calculator's document/application folder.
3. Open the file under the installed Ndless. Do not reset the calculator, update its OS, reinstall Ndless or transfer firmware/boot images for this application.
4. Dismiss the original Eagle splash. Check Settings shows **1.0.2**. Public edition naming does not change the embedded build version.
5. Use arrows and Enter/center-click, or numbered shortcuts. Esc returns through menus and exits at the root.

Fresh installations start in English. Settings → Language offers **1. English** and **2. Turkce ASCII**. The optional 24-byte application-owned `c21-engineering-suite.language.cfg.tns` sidecar is stored beside the executable. It is settings data, not an application. Existing English/ASCII preferences are preserved; old Unicode Turkish preferences load as ASCII and are persisted on the next explicit save. When settings cannot be saved, selection applies to the session. Existing files unrelated to this format are not overwritten. Copying into a new folder without preferences starts in English.

Verify on Windows with `Get-FileHash ./C21_Engineering_Suite_1.0_Classic_Edition.tns -Algorithm SHA256`, or use `sha256sum -c SHA256SUMS.txt` in the release folder. The individual executable checksum is also supplied.

No external logo, language file, internet connection or account is needed at runtime. Only the optional application-owned language preference is written. Preserve the prior application for rollback. Device storage I/O and final V1.0.2 confirmation remain subject to owner acceptance; see [validation](VALIDATION.md).
