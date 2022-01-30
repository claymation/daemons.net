BUILD_DIR := build

default: jlox

# Remove all build outputs and intermediate files.
clean:
	@ rm -rf $(BUILD_DIR)
	@ rm -rf gen

# Compile the Java interpreter .java files to .class files.
jlox:
	@ $(MAKE) -f util/java.make DIR=java PACKAGE=lox

.PHONY: jlox
