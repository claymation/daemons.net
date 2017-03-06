# GNU Make

### Targets

Targets look like this:

```makefile
target(s): prereq(s)
        recipe line(s)
```

Recipe lines are indented with hard tabs.

If the prerequisites for a target are newer than the target, then that target will be rebuilt using the recipe.

Double-colon rules can be used if the recipe required to rebuild a target depends on which prerequisite has changed:

```makefile
target:: foo
        do the thing necessary to rebuild target from foo

target:: bar
        do the thing necessary to rebuild target from bar
```

### Goals

The first target defined in the Makefile is the default goal, but you can pass one or more goals on the command-line instead, in which case they will be built in the order specified (and available in `$(MAKECMDGOALS)`).

### Variables

Use `lower_case` names for private variables, and `UPPER_CASE` names for [variables used by implicit rules](https://www.gnu.org/software/make/manual/html_node/Implicit-Variables.html#Implicit-Variables) (like `CFLAGS`) or for variables meant to be overridden by command-line parameters (like `DEBUG`).

#### Flavors

Make variables come in [two flavors](https://www.gnu.org/software/make/manual/html_node/Flavors.html#Flavors): recursively expanded (`foo = bar`) and simply expanded (`foo := bar`).

Recursively expanded variables are expanded every time they're substituted. Consequently, something like this won't work (because it's infinite recursion):

```makefile
foo = $(foo) bar
```

but this will work (because of the lazy evaluation):

```makefile
foo = $(bar)
bar = blee
```

Simply expanded variables are expanded just once, during the first pass.

#### Conditional definition

To define a variable iff it doesn't already exist, use the conditional definition operator:

```makefile
foo ?= bar
```

#### Overriding

Passing a variable on the command-line overrides any definitions of that variable in the Makefile:

```makefile
FOO = bar
all:
        $(info $(FOO))
```

    $ make
    bar

    $ make FOO=blee
    blee

#### Environment

Environment variables are automatically defined as make variables, unless a variable of the same name is explicitly defined in the Makefile or passed on the make command-line. All make variables are passed as environment variables to recipe sub-shells, except for recursive make invocations, which only receive variables in the parent environment or passed on the command-line (although you can use `export` to communicate variables to sub-makes).

### Includes

Makefiles can include other Makefiles:

```makefile
include other.mk
```

`other.mk` doesn't have to exist, so long as there's a rule to build it. If an included Makefile is rebuilt, [Make will start over from scratch](http://make.mad-scientist.net/constructed-include-files) in order to pick up the changes.
