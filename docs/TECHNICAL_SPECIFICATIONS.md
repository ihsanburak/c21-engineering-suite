> Current release: **1.0.3**. Startup animation added; physical timing/execution remain unverified. Current binary and test evidence: [validation](VALIDATION.md) and [manifest](../dist/build-manifest.json). Earlier 1.0.2 size/test figures below are historical.

# Technical specifications

| Item | Value / scope |
|---|---|
| Public product edition | C21 Engineering Suite 1.0 Classic Edition |
| Application build | 1.0.2; 2026-11-10 |
| Target | Original TI-Nspire Touchpad non-CAS, OS 3.9.0.463, existing compatible Ndless |
| Display | 320×240 monochrome, high-contrast black/white framebuffer |
| Runtime | Native C, ARM, Ndless SDK / nSDL; offline, no account/network service |
| Language | English default / Turkce ASCII; original Unicode Turkish resources retained |
| Coverage | 138 interactive targets / 64 documented families, including one formatter |
| Units | SI internally, explicit selectable engineering input/output units |
| File | Genuine ARM/Ndless `.tns`, 627,568 bytes |
| Optional data | Application-owned 24-byte language preference sidecar |
| GNU size | text 336,348; data 264,816; bss 88,208 bytes for accepted build |
| Loader allocation | 715,284 bytes; not a physical peak-RAM measurement |
| Framebuffer | 76,800 bytes in BSS; SDL/runtime overhead is additional |
| Stack | Earlier same-architecture compiler estimates are planning evidence, not a measured device maximum |

No firmware, CAS, network, telemetry, VNA/radio control or automatic measurement import is provided. Do not interpret desktop sanitizers or binary sections as peak Touchpad RAM measurements. Supported model and physical-evidence limits are documented separately.
