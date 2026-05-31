# Current Task

Update Cycfi Elements from the `skia_2024` baseline to use the latest Cycfi
Artist rendering library from `artist_2026_dev`.

## Working branch

`artist_2026`

## Baseline branch

`skia_2024`

## Baseline commit

`4c64f37d19160f28625561f28ce59c5f8a94c9e0`

## Important clarification

The Artist API is expected to be compatible with the existing Elements
`skia_2024` code. The main new work is enabling and testing the new Cairo
backend.

## Do not do yet

- Do not rewrite rendering abstractions.
- Do not create a compatibility shim unless real API incompatibilities are found.
- Do not change public Elements APIs.
- Do not remove the existing Skia path.
- Do not update Artist yet.
- Do not build yet.
