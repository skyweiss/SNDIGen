# SNDIGen
Command-line tool for combining multiple FSB3 files into a Sound Info asset for The SpongeBob SquarePants Movie (GCN).

Usage:
```
SNDIGen outfile [options] infiles...
```

Options:

- **-s**: Multiplies sample rates by 1124/1125 to correct playback rate in-game.

Outfile will also be the name and ID for the Sound Info asset. Since these are streamed sounds, do not change the name or ID afterwards.

Infiles should be FSB3 files. These are simply copied and pasted into the Sound Info asset.

## Importing

It is not recommended to import the Sound Info asset into mnuc.hip, as this will usually cause a multitude of issues, including lack of audio playback. Use a different globally loaded file instead, like mnuc_US.hip.

Adding SNDS assets for each individual sound is not necessary, although you may want to do this anyway, solely so Industrial Park will autofill the name of the sound when using it in other asset properties and links.
