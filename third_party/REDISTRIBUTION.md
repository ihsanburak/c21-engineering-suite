# Source, licensing and static relinking materials

The .tns statically links nSDL/SDL, Ndless runtime, Newlib and GCC runtime, with the official uncompressed compatibility loader. Each retains its terms; original C21 code is MIT. Preserve this document, THIRD_PARTY_NOTICES.md, LICENSE_SCOPE.md, license copies, sources and relink materials when redistributing this release.

## Included source and provenance

- sources/nSDL-cf4c07a-source.zip: original core source/headers, Makefile.nsp and notices from https://github.com/Vogtinator/nSDL at cf4c07a6c8ae3c75e058d460763587307d5683cb. The SDK commit 052f0ac470b578e866956a540236f83c34b8fe99 explicitly identifies this revision for its libSDL buffer-fix rebuild; subsequent fba0e804f36464596ed686dc32a2abe69c4052a5 records a GCC10.2/Newlib4.0 rebuild. The generic hoffa 1.1.1 tag is not substituted for the identified source.
- sources/Ndless-37d61aa-sdk-source.zip: pinned source for libndls, libsyscalls, include/system and the uncompressed loader, with upstream root README/MPL license. MPL-covered source is supplied under its existing terms, not MIT. Directory/per-file exceptions and notices remain intact.
- ../relink/objects/: all twelve C21 ARM objects for the accepted build; ../relink/relink.sh relinks them with a rebuilt or modified libSDL.a using the pinned SDK and normal runtime libraries.
- licenses/: LGPL2.1, MPL1.1, GCC Runtime Library Exception3.1/GPL3, Newlib per-component notices, SDK declarations, BSD notices and validation-only Semtech attribution.

This supplies library source/build material and application objects for the LGPL2.1 section6(a) route; no future written offer or external source link alone is relied upon. Recipients may modify/debug/relink covered software under its licenses. No EULA or brand notice overrides those rights. Keep full upstream notices and source availability with redistributed binaries. Do not relabel modified/relinked builds as the owner's accepted executable.

## Rebuild and relink

Use the official pinned Ndless SDK commit 37d61aa22762171ffc9b0abf2565f13d09aec70f and a compatible GNU ARM toolchain. Set NDLESS_SDK to your SDK; add its bin/ and toolchain/install/bin/ to PATH. Unpack the nSDL source to a working directory and run `make -f Makefile.nsp`. Set NSDL_ARCHIVE to the absolute path of that rebuilt or modified libSDL.a, then `bash relink/relink.sh` from the repository. It writes only a fresh relink/output/ and refuses to overwrite it. The normal pinned SDK supplies libndls, syscalls, Newlib and GCC runtime; included SDK source permits rebuilding covered runtime parts. No firmware, private boot images, SDK installer or proprietary font is required or included. For loader-only rebuild, use the included SDK source and `make zehn_loader.tns` within ndless-sdk/tools/zehn_loader (not the compressed/all target).

Technical checks: supplied nSDL source builds with GCC14.2 and reproduces all49 object members of the review rebuild (archive metadata differs); modifying SDL.c and relinking the original objects succeeds; supplied loader source reproduces the exact492-byte official loader. The SDK's older GCC10.2 archive is not claimed byte-reproduced. The accepted application remains unchanged; its own clean-source build is byte-identical in the pinned installed environment. Modern GCC emits warnings in the old vendor source; these are not silently fixed or claimed as new clean app regression results. Test-only relink documents are not distributed as the release executable.

Newlib/GCC compiler-runtime notices apply separately. The build used the standard GNU GCC toolchain; no incompatible GCC intermediate-representation plugin was used. Semtech driver source is validation-only and not linked. No source/notice requirement for unlinked SDK libraries is inferred merely from their installation.
