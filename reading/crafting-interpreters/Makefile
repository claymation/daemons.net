BUILD_DIR := build

default: jlox

# Remove all build outputs and intermediate files.
clean:
	@ rm -rf $(BUILD_DIR)
	@ rm -rf gen

# Compile and run the AST generator.
generate_ast:
	@ $(MAKE) -f util/java.make DIR=java PACKAGE=tool
	@ java -cp build/java com.craftinginterpreters.tool.GenerateAst \
			java/com/craftinginterpreters/lox

# Compile the Java interpreter .java files to .class files.
jlox: generate_ast
	@ $(MAKE) -f util/java.make DIR=java PACKAGE=lox

.PHONY: jlox
