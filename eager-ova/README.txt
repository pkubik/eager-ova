# One-Versus-All Classification Rule Miner

The compiled rule miner consists of a single executable `rule-miner.exe`
which accepts a single command line parameter to specify the *data directory*.

## The *data directory* convention

All of the paths described below are relative to the *data directory*. For
details please refer to example in `sample-data` subdirectory.

- `rule-miner-params.txt` - (required) rule mining parameters
- `data.csv` - (required) input data in a table stored as comma separated values
- `rules.txt` - (not required) will contain extracted rules

## Contrastive patterns

By default the program finds all contrastive patterns within a given datasets.
Exploration can be controlled by a single parameter `minSupport` which denotes
minimal relative support of the rules and should be decreased in order to
speed up the mining process.

## Decission rules with growth threshold

The program can be run in a second mode by specifying `cp = 0` in the configuration file.
In this mode the rule confidence is ignored and the program generated the decission rules.
The number of rules can be limited to those satisfying `growthThreshold` set in the
configuration file.

An example configuration file deticated for exploring decission rules with growth threshold
is stored within the `car-data` subdirectory.