# tree-sitter-sqf

Arma SQF language grammar for [tree-sitter](https://github.com/tree-sitter/tree-sitter)

## What is SQF?

It's a scripting language for the Arma series of video games by Bohemia Interactive. References used for creating this grammar were:

- [SQF Syntax](https://community.bistudio.com/wiki/SQF_Syntax)
- [Scripting Commands](https://community.bistudio.com/wiki/Category:Arma_3:_Scripting_Commands)
- [PreProcessor Commands](https://community.bistudio.com/wiki/PreProcessor_Commands)
- [Order of Precedence](https://community.bistudio.com/wiki/Order_of_Precedence)

as well as heavy testing through the integrated debug console in Arma 3.

## What can I use this for?

It's meant to allow creating high quality formatters, syntax highlighters and linters for SQF. You could also use it to create an interpreter or compiler for SQF, but note that it does not parse preprocessor macros correctly, you may have to run a preprocessor on the file first or limit yourself to only a subset of the preprocessor features.

## What language features it supports?

- Pretty much all SQF features and commands up to Arma 3 2.20.

- Most use cases of preprocessor macros, as long as they behave like expressions or statements.

## What language features it does not support?

##### New commands

- Newer Arma versions may introduce new commands that are not automatically supported. The command list in grammar.js needs to be updated.

##### Custom commands

- Custom SQF commands [registered with Intercept](https://github.com/intercept/intercept/wiki/Registered-Functions) are not supported. They would also need to be added to the command list in grammar.js.

##### Syntax breaking macros

- Preprocessor macros that break the SQF syntax, such as expanding a macro to an opening brace while the closing brace already exists in the code. Example:
  ```sqf
  #define OPENING_BRACE {
  call OPENING_BRACE variable = 5; };
  ```
  While this is valid SQF, it needs a real preprocessor to handle correctly.

##### Complicated #if #else macros

- #if #else preprocessor macros where the code in the #if and #else sections could not be concatenated, such as conditionally changing the value of a variable assignment. Example:
  ```sqf
  variable =
  #ifdef MACRO
  5
  #else
  2
  #endif
  ;
  ```
  This is because the parser is simply treating the macros as extras and trying to concatenate the code in both sections. It could possibly be made to ignore the code in the #else section, but then that would not be parsed at all. Instead you could do this:
  ```sqf
  // This is supported
  #ifdef MACRO
  variable = 5;
  #else
  variable = 2;
  #endif
  ```

##### Strange newline escapes in macros

- Escaped newlines in preprocessor macros in some positions, such as inside macro names, macro variables or between the arguments of a define macro. Examples:
  ```sqf
  #defi\
  ne MACRO definition
  #define MAC\
  RO definition
  #define MACRO_ARGS(\
  a,\
  b\
  ) definition
  ```
  These are all valid macros in SQF, but the rules for where newline escaping is allowed and where not are so weird and inconsistent that they were just left unimplemented. Escaped newlines are supported only between different parts of a macro and in their definitions, for example:
  ```sqf
  // These are supported newline escapes
  #define \
  MACRO \
  defi\
  nition
  #if \
  3 \
  == \
  3
  #endif
  ```

  <details>
  <summary>(What do I mean by weird and inconsistent?)</summary>

  Well why is this a valid macro in SQF,
  ```sqf
  #define MACRO_ARGS(a\
   ,b) a + b
  ```
  but this is not,
  ```sqf
  #define MACRO_ARGS(a,\
   b) a + b
  ```
  and this again is?
  ```sqf
  #define MACRO_ARGS(a,\
  b) a + b
  ```
  How is one supposed to know all the valid arrangements of escaped newlines and spaces?

  </details>

##### Comments inside tokens

- Comments inside variables, commands, numbers etc. Example:
  ```sqf
  vari/*comment*/able;
  ```
  As weird as it sound, this is valid SQF and becomes `variable;` after preprocessing, but again you need a real preprocessor to handle it.

##### Variable and macro call difference

- Telling apart SQF variables and macro calls without arguments. This is simply not possible at the parsing stage as there is no difference in how they look. They are both classified as `variable`. Macro calls with arguments look different and they are classified as `macro_call`.

##### Macro calls named after commands

- If you call a macro with the same name as a known SQF command, it will be parsed as a command and you will get errors. The parser cannot know that you have replaced a command with a macro in your code.

##### Variables named after new commands

- You can use this parser to parse SQF for older Arma versions, but with one caveat: If you use a variable name which is a valid command in later Arma versions, it will be parsed as a command and you will get errors. There is no way to know which Arma version the code should be parsed for.

## Architectural notes

- All valid commands are listed in the bottom section of the grammar.js. We must list them all and can't allow unknown commands because they affect parsing. Imagine if we allowed commands to have any name. Now the following sequence
  ```sqf
  a b c d;
  ```
  has at least 3 different interpretations:
  ```sqf
  (variable binary_command (unary_command variable))
  ((unary_command variable) binary_command variable)
  (unary_command (unary_command (unary_command variable)))
  ```
  The only way to know which of those is correct is to know all the command names and which class they belong to.

- An external scanner was used for two things:
  - To detect if a hash sign is at the beginning of a line to determine whether it should begin a macro.
  - To force a certain kind of whitespace inside macros even when normal whitespace is in the extras.

- There are currently 11 failing tests that are unlikely to ever get fixed. They all correspond to some of the unsupported features listed above.

- The parser aims to support all valid SQF but does not aim to throw errors on all invalid SQF. There are many cases of invalid SQF that it parses just fine.
