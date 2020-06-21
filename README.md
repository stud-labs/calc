# calc
flex and bison calculator demonstrating polymorphism in the abstract syntax tree

The original code comes from "flex & bison". I though that I would implement the abstract syntax tree with a polymorphic class.

In order to build 'calc' from qt creator you'll need to add two custom build steps. One for bison and one for flex. These build steps have to happen before anything else and the working directory is %{sourceDir} for both build steps. The commands are in Makefile.orig.
