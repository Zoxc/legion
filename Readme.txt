Legion 0.0.4
© Zoxc 2010

Legion is a parser that will validate the syntactic structure of a Galaxy script.
You simply pass the Galaxy scripts (without file extensions) to the program and
it will parse it and follow includes. Includes are looked for in the current folder.

The grammar uses is a LL(1) superset of the Galaxy grammar so it might allow
things SC2 will not. It's semantic analysis isn't complete so I suggest you chain
it with something like LAPIN's Galaxy syntax validator.