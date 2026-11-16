# io/
Reading the transaction stream and printing results.

- `InputReader.h/.cpp`: reads `label: value` lines from standard input and checks each label is the one expected
- `ResultPrinter.h/.cpp`: prints the four-line result block (key value, Status, Message, outcome) to standard output