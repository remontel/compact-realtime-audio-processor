# U8g2 dependency

- Release: [2.36.19](https://github.com/olikraus/u8g2/releases/tag/2.36.19)
- Source: https://codeload.github.com/olikraus/u8g2/tar.gz/refs/tags/2.36.19
- Imported files: complete `csrc/` directory, upstream `README.md` and `LICENSE` (138 files).
- Local modifications to upstream files: none.
- Verification: all 138 files matched the release archive byte for byte on November 13, 2026.
- Downloaded archive SHA-256: `ff64b9b5a2b43f2beac320673292daeb59d1624aa60fe351499a6bfa695b8c0c`.

The F411 target compiles the C sources through its top-level CMake file. Keep this source set together for reproducible builds. Any later reduction should update the build source list and pass a clean build. Preserve the upstream license and source/font notices.
