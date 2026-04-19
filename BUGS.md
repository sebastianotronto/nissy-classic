# Known bugs and issues

I will fix these at some point.

## Incomplete filtering when using NISS

When using NISS, sometimes the switch happens when it does not make sense.
For example, for EO:

```
nissy solve eoud -a -N -m 5 -M 5 -p "R' U' F R D L' B' L2 U2 R' B U2 B2 R2 B2 D' F2 U D2 R2 D B2 R U R' U' F"
```

The last 4 EOs returned by this command start with `D2 U (switch...)`:

```
...
D2 U (L F D)
D2 U (L F D')
D2 U' (L F D)
D2 U' (L F D')
```

These EOs should not be included, because the starting `D2` move is useless.

It is possible that this issue affects other steps too.

## Optimal solver duplicate solutions

There is an issue with move ordering that leads to the same solution occasionally being listed
twice.

To reproduce:

```
nissy solve optimal -o "R' F' D2 L' U' L B D L2 F2 L' U2 R' F2 R' D2 F2 U2 F'"
```

Output:

```
U' D2 F B' U R' U D B R' B' U' L D2 F2 B2 L (17)
U' D2 F B' U R' U D B R' B' U' L D2 B2 F2 L (17)
```

The two solutions are the same, but with `F2 B2` swapped.
