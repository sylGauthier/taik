# taik

Small, arcade 3D spaceship game. You control a small spaceship with the mouse,
you have to avoid all the randomly generated walls and max your score.

Each level correspond to a seed. To progress to next level, you need to get at
least 50 times your level in points, so progressing become increasingly hard.

It's in alpha, not really actively developed. It is a proof-of-concept and demo
for our [3dmr](https://pedantic.software/projects/3dmr.html) minimal game
engine.

![screenshot](https://pedantic.software/syg/files/taik/taik.png)

## Install

You need to install [3dmr](https://pedantic.software/projects/3dmr.html) first:

```bash
git clone https://pedantic.software/git/3dmr
cd 3dmr
PREFIX=/usr sudo make install
```

Once `3dmr` is installed:

```bash
git clone https://pedantic.software/git/taik
cd taik
make
```

## Play

Launch with `./taik` in the root directory of the repo, once built. Move your
mouse to shift the ship left and right. Avoid all walls, including the ones on
the side.

The score is displayed in the window's title bar.
