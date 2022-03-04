FrequencySummoner Emphasis module

Currently this is a patch for kxmx_bluemchen. If things go well, maybe
it will become a Daisy patch_sm-based module.

To build:

symlink kxmx_bluemchen project into this directory (including libDaisy
and DaisySP submodules), or set LIBDAISY_DIR, DAISYSP_DIR and
BLUEMCHEN_DIR variables to point to those project folders.

Then you can build with the makefile, e.g. `sudo make program-dfu`.
My usual build command is
`make TARGET=emphasis && sudo make TARGET=emphasis program`
to flash with STLink, with the option to
choose other targets (i.e. prototype/test programs.)
