# Third-party notices — Version 1.0 Classic Edition

These notices retain separate upstream terms. Pinned source, license copies and application relink materials are included under third_party/ and relink/. They do not relicense third-party code.

| Component | Terms / attribution / required treatment |
|---|---|
| C21 application, documentation, translations and custom bitmap font | MIT licensed for original C21-owned code with owner authorization. The font generator specifies original 5×9 patterns without system-font extraction. Technical observations do not prove legal title. |
| Eagle / C21 LABS names | Separately reserved brand artwork/trademarks, excluded from the MIT code license. Owner confirmed original Eagle ownership and authorized this project distribution; master files remain absent, and no logo was changed or reconstructed. |
| nSDL / SDL | LGPL2.1-or-later per source notices; port by Christoffer Rehn and later contributors, SDL by Sam Lantinga and contributors. Identified fork/revision and source/build/relink materials accompany the component package. Preserve upstream notices and recipient modification/relink rights. |
| Ndless libndls | Mixed per-file/default MPL1.1, including explicit Touchpad/wait/other routines. Preserve authors' source notices (including Goplat/Olivier Armand/Fabian Vogt and other named contributors). Public-domain include/system declarations do not relicense this directory. |
| Ndless libsyscalls | Source and generated stubs retained; apply repository-default MPL1.1 where no more-specific notice exists. realpath.c has a specific BSD notice, preserved even though realpath.o does not contribute to this accepted binary. Do not label the entire archive public domain. |
| Ndless headers/system startup | Published directory-specific public-domain declarations, with more-specific notices controlling included third-party files. Copies are retained. |
| Uncompressed compatibility loader | Repository-default MPL1.1 classification per upstream README; no contrary local exception observed. Source has no local license header, so retain root README/license, exact source rather than inventing a new per-file grant. Exact official 492-byte source rebuild verified. |
| Newlib 4.5.0.20241231 | Preserve the full applicable per-component copyright, conditions and disclaimer texts in Newlib-notices.txt. No universal BSD relicense is asserted. |
| GCC14.2 runtime | GPLv3 with GCC Runtime Library Exception3.1 where applicable. Preserve the exception/license; use the applicable eligible-compilation conditions (the build used the standard GNU toolchain). This does not automatically license C21 code under GPL. |
| SDK keys fixture | Exact SDK key constants in host test stubs; include directory declaration retained. Test SDL/OS boundaries are original harness code, not actual device emulation. |
| Semtech validation | BSD reference drivers used only as pinned host oracles; no driver body linked or included in the app source. Numeric references/provenance and original BSD attribution are retained. |
| Development-only tools | GCC, TinyCC, Python/Pillow, WSL and optional Inkscape are not runtime dependencies shipped as installers/binaries. No system font files are bundled. |

The accepted map extracts members from libSDL.a, libndls.a, libsyscalls.a, libc.a, libm.a and libgcc.a. nSDL built-in fonts, SDL_image, FreeType, zlib and nspire-io are not claimed linked. The included component source ZIPs retain per-file notices. License copies, source/build instructions and all twelve original application objects accompany the release/source package. See third_party/REDISTRIBUTION.md; source links are attribution, not a replacement for the supplied materials.

Independent project. Not affiliated with, sponsored by or endorsed by Texas Instruments or the Ndless project. TI-Nspire is a Texas Instruments trademark. Third-party names identify dependencies, not endorsements. No TI firmware, private boot image, Ndless installer or proprietary font is distributed.
