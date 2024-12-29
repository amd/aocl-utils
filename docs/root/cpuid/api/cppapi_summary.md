# C++-API Behaviour Summary

## Behaviour of `X86Cpu.isUarch`

X86Cpu.isUarch supports two arguments.
- EUarch - Micro Architecture {Zen, ZenPlus, Zen2, Zen3, Zen4, Zen5}
- strict - Bool {True/False}

With strict argument set to "False" which is the default behaviour.

| AOCL 5.0.1 | isUarch(Zen) | isUarch(ZenPlus) | isUarch(Zen2) | isUarch(Zen3) | isUarch(Zen4) | isUarch(Zen5) |
| :--------: | :----------: | :--------------: | :-----------: | :-----------: | :-----------: | :-----------: |
|  Skylake   |    False     |      False       |     False     |     False     |     False     |     False     |
| Bulldozer  |    False     |      False       |     False     |     False     |     False     |     False     |
|    Zen1    |     True     |      False       |     False     |     False     |     False     |     False     |
|   Zen1+    |     True     |       True       |     False     |     False     |     False     |     False     |
|    Zen2    |     True     |       True       |     True      |     False     |     False     |     False     |
|    Zen3    |     True     |       True       |     True      |     True      |     False     |     False     |
|    Zen4    |     True     |       True       |     True      |     True      |     True      |     False     |
|    Zen5    |     True     |       True       |     True      |     True      |     True      |     True      |
|  Zen[X>5]  |    False     |      False       |     False     |     False     |     False     |     False     |

With strict argument set to "True".

| AOCL 5.0.1 | isUarch(Zen,1) | isUarch(ZenPlus,1) | isUarch(Zen2,1) | isUarch(Zen3,1) | isUarch(Zen4,1) | isUarch(Zen5,1) |
| :--------: | :------------: | :----------------: | :-------------: | :-------------: | :-------------: | :-------------: |
|  Skylake   |     False      |       False        |      False      |      False      |      False      |      False      |
| Bulldozer  |     False      |       False        |      False      |      False      |      False      |      False      |
|    Zen1    |      True      |       False        |      False      |      False      |      False      |      False      |
|   Zen1+    |     False      |        True        |      False      |      False      |      False      |      False      |
|    Zen2    |     False      |       False        |      True       |      False      |      False      |      False      |
|    Zen3    |     False      |       False        |      False      |      True       |      False      |      False      |
|    Zen4    |     False      |       False        |      False      |      False      |      True       |      False      |
|    Zen5    |     False      |       False        |      False      |      False      |      False      |      True       |
|  Zen[X>5]  |     False      |       False        |      False      |      False      |      False      |      False      |

## Behaviour of `Cpu.isUarch` API for AOCL 4.2

Cpu.isUarch supports one argument

- Uarch - Micro Architecture {eZen, eZen2, eZen3, eZen4}

| AOCL 4.2  | isUarch(Zen) | isUarch(Zen2) | isUarch(Zen3) | isUarch(Zen4) |
| :-------: | :----------: | :-----------: | :-----------: | :-----------: |
|  Skylake  |    False     |     False     |     False     |     False     |
| Bulldozer |    False     |     False     |     False     |     False     |
|   Zen1    |     True     |     False     |     False     |     False     |
|   Zen1+   |     True     |     False     |     False     |     False     |
|   Zen2    |    False     |     True      |     False     |     False     |
|   Zen3    |    False     |     False     |     True      |     False     |
|   Zen4    |    False     |     False     |     False     |     True      |
|   Zen5    |    False     |     False     |     False     |     True      |
| Zen[X>5]  |    False     |     False     |     False     |     True      |
