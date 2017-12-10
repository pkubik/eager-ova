# One-Versus-All Classification Rule Miner

The compiled rule miner consists of a single executable `rule-miner.exe`
which accepts a single command line parameter to specify the *data directory*.

## The *data directory* convention

All of the paths described below are relative to the *data directory*. For
details please refer to example in `sample-data` subdirectory.

- `rule-miner-params.txt` - (required) rule mining parameters
- `data.csv` - (required) input data in a table stored as comma separated values
- `rules.txt` - (not required) will contain extracted rules
